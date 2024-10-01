#include <ws2tcpip.h>
#if DEBUG
#include <time.h>
#endif
#include "main.h"
#include "stdio.h"

#if ENABLE_ENCRYPTION
	#include "cry.h"
#endif

int sendE(SOCKET s, const char* buf, int len, int flags) {
	
	#if ENABLE_ENCRYPTION
	
	char* newBuff = (char*)calloc(len, sizeof(char));
	if (!newBuff) { return -1; }
	cry_buff(buf, newBuff, len, 0, ENCRYPTION_SHIFT_NUM);
	int r = send(s, newBuff, len, flags);
	free(newBuff);
	return r;

	#else

	return send(s, buf, len, flags);
	
	#endif
}

int recvE(SOCKET s, char* buf, int len, int flags) {
	
	goto_recv:
	
	#if ENABLE_ENCRYPTION
	
	if (buf == NULL || len == 0) { return recv(s, buf, len, flags); }
	char* newBuff = (char*)calloc(len, sizeof(char));
	int r = recv(s, newBuff, len, flags);
	if (r == SOCKET_ERROR || r == 0) { free(newBuff); return r; }
	cry_buff(newBuff, buf, r, 1, ENCRYPTION_SHIFT_NUM);
	free(newBuff);
	
	#else
	
	int r = recv(s, buf, len, flags);
	
	#endif
	
	if (!strncmp(PING_STRING, buf, strlen(PING_STRING))) {
		D(printf("ping %ld\n", time(0)));
		memset(buf, 0, len);
		goto goto_recv;
	}
	
	return r;
}