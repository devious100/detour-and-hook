// LOADER.CPP
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
// Include the Detours Header (It will include the Library on its own.):
#include "detours.h"

int main(int argc, char* argv[], char* envp[])
{
 	// Set Target:
	char szTargetCommand[]		= "Game.exe";
	char szTargetApp[]			= "G:\\Game\\Game.exe";
	char szTargetFolder[]		= "G:\\Game";
	// Aquire Full Path to our Dynamic Link Library:
	char szUseLibrary[MAX_PATH];
	_fullpath(szUseLibrary, "HookLibrary.dll", MAX_PATH);

	// Set the Correct Working Directory:
	SetCurrentDirectory(szTargetFolder);

	// Create Process & Inject:
	printf("Creating Process for %s and Injecting %s.\n",strrchr(szTargetApp,(int)'\\')+1,strrchr(szUseLibrary,(int)'\\')+1);
	STARTUPINFO				si;	ZeroMemory( &si, sizeof(si) );		
	PROCESS_INFORMATION		pi;	ZeroMemory( &pi, sizeof(pi) );
	si.cb = sizeof(si);
    if(!DetourCreateProcessWithDll(	szTargetApp, szTargetCommand, 0, 0, TRUE, 
									CREATE_DEFAULT_ERROR_MODE|CREATE_NEW_CONSOLE, NULL, 
									szTargetFolder,&si, &pi, szUseLibrary, 0)) {
		printf("Loader.exe: DetourCreateProcessWithDll failed: %d\n", GetLastError());
		return 0;
    }
	printf("Injection Successful. Will exit when target exits.\n");
	// Loader will standby until the injected target ends.
	WaitForSingleObject(pi.hProcess, INFINITE);
	printf("Target Ended.\n");
	// We're Finished.
	return 0;
}