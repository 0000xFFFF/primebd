#include <windows.h>
#include <unistd.h>

#include "main.h"

#include "shell.h"

static SERVICE_STATUS ServiceStatus;
static SERVICE_STATUS_HANDLE hStatus;
// Control handler function
static void ControlHandler(DWORD request) {
	switch(request) {
		case SERVICE_CONTROL_STOP:
		case SERVICE_CONTROL_SHUTDOWN:
			// PRINT: SERVICE STOPED
			ServiceStatus.dwWin32ExitCode = 0;
			ServiceStatus.dwCurrentState  = SERVICE_STOPPED;
			#if SHELL_SERVICE_ALLOWSTOP
			SetServiceStatus(hStatus, &ServiceStatus); // comment this line to prevent stopping from SC/services
			#endif
			return;
		
		default: break;
	}
	
	// Report current status
	SetServiceStatus(hStatus, &ServiceStatus);
	
	return;
}
static void ServiceMain(int argc, char** argv) {
	UNUSED(argc);
	UNUSED(argv);
	
	ServiceStatus.dwServiceType             = SERVICE_WIN32;
	ServiceStatus.dwCurrentState            = SERVICE_START_PENDING;
	ServiceStatus.dwControlsAccepted        = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
	ServiceStatus.dwWin32ExitCode           = 0;
	ServiceStatus.dwServiceSpecificExitCode = 0;
	ServiceStatus.dwCheckPoint              = 0;
	ServiceStatus.dwWaitHint                = 0;
	hStatus = RegisterServiceCtrlHandler(SHELL_SERVICE_NAME, (LPHANDLER_FUNCTION)ControlHandler);
	if (hStatus == (SERVICE_STATUS_HANDLE)0) { return; } // Registering Control Handler failed
	
	// PRINT: SERVICE START
	
	// We report the running status to SCM. 
	ServiceStatus.dwCurrentState = SERVICE_RUNNING; 
	SetServiceStatus (hStatus, &ServiceStatus);
	
	while (ServiceStatus.dwCurrentState == SERVICE_RUNNING) {
		const char* cwd = getcwd(NULL, 0); // set dir backup
		shell_main(SHELL_MODE_SERVICE);
		chdir(cwd); // get back to default dir
		Sleep(SHELL_SERVICE_SLEEP);
	}
	return; 
}

BOOL shell_service_start() {
	SERVICE_TABLE_ENTRY ServiceTable[2];
	ServiceTable[0].lpServiceName = SHELL_SERVICE_NAME;
	ServiceTable[0].lpServiceProc = (LPSERVICE_MAIN_FUNCTION)ServiceMain;
	ServiceTable[1].lpServiceName = NULL;
	ServiceTable[1].lpServiceProc = NULL;
	// Start the control dispatcher thread for our service
	return StartServiceCtrlDispatcher(ServiceTable);
}

BOOL shell_service_stop() {
	ServiceStatus.dwWin32ExitCode = 0;
	ServiceStatus.dwCurrentState  = SERVICE_STOPPED;
	return SetServiceStatus(hStatus, &ServiceStatus);
}