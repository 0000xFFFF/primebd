#include <stdio.h>
#include <windows.h>

#include "main.h"

static DWORD recordWavFile(DWORD dwMilliSeconds) {
	
	D(
		printf("recordWavFile(%ld)\n", dwMilliSeconds);
	);
	
	DWORD dwReturn;
	
	// Open a waveform-audio device with a new file for recording.
	MCI_OPEN_PARMS mciOpenParms;
	mciOpenParms.lpstrDeviceType = "waveaudio";
	mciOpenParms.lpstrElementName = "";
	DPL("mciSendCommand MCI_OPEN");
	dwReturn = mciSendCommand(0, MCI_OPEN, MCI_OPEN_ELEMENT | MCI_OPEN_TYPE, (DWORD_PTR)&mciOpenParms);
	if (dwReturn) { DPL("mciSendCommand MCI_OPEN failed"); return dwReturn; }
	
	// The device opened successfully; get the device ID.
	UINT wDeviceID = mciOpenParms.wDeviceID;
	
	// Begin recording and record for the specified number of 
	// milliseconds. Wait for recording to complete before continuing. 
	// Assume the default time format for the waveform-audio device 
	// (milliseconds).
	MCI_RECORD_PARMS mciRecordParms;
	mciRecordParms.dwTo = dwMilliSeconds;
	DPL("mciSendCommand MCI_RECORD");
	dwReturn = mciSendCommand(wDeviceID, MCI_RECORD,  MCI_TO | MCI_WAIT, (DWORD_PTR)&mciRecordParms);
	if (dwReturn) { DPL("mciSendCommand MCI_RECORD failed"); mciSendCommand(wDeviceID, MCI_CLOSE, 0, 0); return dwReturn; }
	
	/*
	// play recording
	MCI_PLAY_PARMS mciPlayParms;
	mciPlayParms.dwFrom = 0L;
	dwReturn = mciSendCommand(wDeviceID, MCI_PLAY, MCI_FROM | MCI_WAIT, (DWORD)(LPVOID) &mciPlayParms);
	if (dwReturn) { mciSendCommand(wDeviceID, MCI_CLOSE, 0, 0); return dwReturn; }
	*/
	
	// wait/save
	MCI_SAVE_PARMS mciSaveParms;
	mciSaveParms.lpfilename = "rec.wav";
	DPL("mciSendCommand MCI_SAVE");
	dwReturn = mciSendCommand(wDeviceID, MCI_SAVE, MCI_SAVE_FILE | MCI_WAIT, (DWORD_PTR) &mciSaveParms);
	if (dwReturn) { DPL("mciSendCommand MCI_SAVE failed"); mciSendCommand(wDeviceID, MCI_CLOSE, 0, 0); return dwReturn; }
	
	// close device
	mciSendCommand(wDeviceID, MCI_CLOSE, 0, 0);
	return dwReturn;
}
DWORD recMic(const char* ms) { return recordWavFile(atoi(ms)); }

static DWORD WINAPI recMic_bg_thread(void* data) {
	
	char* ms = (char*)data;
	int mscl = strlen(ms) + 1;
	char msc[mscl];
	memset(msc, 0, mscl);
	strcpy(msc, ms);
	free(ms);
	return recMic(msc);
}
HANDLE recMic_bg(const char* ms) {
	return CreateThread(NULL, 0, recMic_bg_thread, (void*)strdup(ms), 0, NULL);
}