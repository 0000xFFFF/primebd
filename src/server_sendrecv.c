#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <winsock2.h>

#include "main.h"
#include "cry_socket.h"

SOCKET server_init() {
	// initialize winsock
	WSADATA wsData;
	if (WSAStartup(MAKEWORD(2, 2), &wsData) != 0) { fprintf(stderr, "ERROR: failed to initialize winsock\n"); return INVALID_SOCKET; }
	
	// create socket
	//SOCKET sock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) { fprintf(stderr, "ERROR: failed to create socket\n"); return 1; }
	
	// set timeout (can't use WSASocket() must use socket())
	DWORD timeout = RECV_TIMEOUT;
	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
	
	// bind socket to ip:port
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.S_un.S_addr = INADDR_ANY;
	addr.sin_port = htons(HOST_PORT_SENDRECV);
	
	// bind & listen
	if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) { fprintf(stderr, "ERROR: failed to bind socket\n"); closesocket(sock); return INVALID_SOCKET; }
	if (listen(sock, SOMAXCONN) == SOCKET_ERROR)                           { fprintf(stderr, "ERROR: failed to listen\n");      closesocket(sock); return INVALID_SOCKET; }
	
	printf("# Waiting for a connection...\n");
	// wait for a client
	struct sockaddr_in client;
	memset(&client, 0, sizeof(client));
	int clientSize = sizeof(client);
	SOCKET clientSocket = accept(sock, (struct sockaddr*)&client, &clientSize);
	if (clientSocket == INVALID_SOCKET) { fprintf(stderr, "ERROR: failed to accept %d\n", WSAGetLastError()); closesocket(sock); return INVALID_SOCKET; }
	printf("# %s:%d\n", inet_ntoa(client.sin_addr), htons(client.sin_port));
	closesocket(sock);
	
	// set timeout for recv
	setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
	
	return clientSocket;
}

void server_shutdown(SOCKET sock) {
	closesocket(sock);
	WSACleanup();
}

int main_send(const char* filePath) {
	
	printf("# Opening file...\n"); 
	FILE* f = fopen(filePath, "rb");
	if (!f) { fprintf(stderr, "ERROR: failed to open file\n"); return 0; }
	
	SOCKET sock = server_init();
	
	printf("# Sending file...\n"); 
	char buff[KSIZ] = {0};
	int result = 0;
	//size_t total = 0;
	while ((result = fread(buff, 1, KSIZ, f)) > 0) {
		int r = 0;
		if ((r = sendE(sock, buff, result, 0)) == SOCKET_ERROR) {
			fprintf(stderr, "ERROR: send failed\n");
			fclose(f);
			return 0;
		}
		

		/* PRINT INFO
		total += r;
		#ifdef _WIN64
			printf("\rsent: %lld", total);
		#else
			printf("\rsent: %d", total);
		#endif
		//*/
	}
	
	fclose(f);
	server_shutdown(sock);
	return 1;
}

int main_recv(const char* filePath) {
	
	printf("# Opening file...\n"); 
	FILE* f = fopen(filePath, "wb");
	if (!f) { fprintf(stderr, "ERROR: failed to open file\n"); return 0; }
	
	SOCKET sock = server_init();
	
	printf("# Receiving file...\n"); 
	char buff[KSIZ] = {0};
	//size_t total = 0;
	int r = 0;
	while ((r = recvE(sock, buff, KSIZ, 0)) > 0) {
		if (r == 0) { fprintf(stderr, "WARNING: Client disconnected\n"); break; }
		if (r == SOCKET_ERROR) {
			fprintf(stderr, "ERROR: recv failed\n");
			fclose(f);
			return 0;
		}
		fwrite(buff, sizeof(char), r, f);

		/* PRINT INFO
		total += r;
		#ifdef _WIN64
			printf("\rrecv: %lld", total);
		#else
			printf("\rrecv: %d", total);
		#endif
		//*/
	}
	
	fclose(f);
	server_shutdown(sock);
	return 1;
}

int main(int argc, char** argv) {
	
	if (argc < 3) {
		printf("USAGE: %s <mode> <file>\n", argv[0]);
		printf(" mode: 1 - send, 2 - recv");
		return 1;
	}
	
	int mode = atoi(argv[1]);
	switch (mode) {
		case 1: break;
		case 2: break;
		default: fprintf(stderr, "ERROR: invalid mode\n"); return 1; break;
	}
	char* filePath = argv[2];
	
	switch (mode) {
		case 1: main_send(filePath); break;
		case 2: main_recv(filePath); break;
	}
	
	
	return 0;
}