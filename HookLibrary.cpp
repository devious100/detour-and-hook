// HOOKLIBRARY.CPP
#include <windows.h>
#include <stdio.h>
#include <detours.h>
#include <GL/gl.h>
#include <d3d9.h>

static FILE* fLogFile = NULL;

typedef void		(APIENTRY *FUNC_GLBEGIN)			(GLenum mode);
typedef IDirect3D9*	(APIENTRY *FUNC_DIRECT3DCREATE9)	(UINT SDKVersion);

FUNC_DIRECT3DCREATE9 Real_Direct3DCreate9 = NULL;
IDirect3D9* APIENTRY Mine_Direct3DCreate9(UINT SDKVersion)
{
	if(fLogFile) {
		fprintf(fLogFile, "Direct3DCreate9(%i)\n", SDKVersion);
		fflush(fLogFile);
	}
	
	return Real_Direct3DCreate9(SDKVersion);
}

FUNC_GLBEGIN Real_glBegin = NULL;
void APIENTRY Mine_glBegin(GLenum mode)
{
	if(fLogFile) {
		fprintf(fLogFile,"glBegin(%d)\n",mode);
		fflush(fLogFile);
	}
	Real_glBegin(mode);
}

BOOL APIENTRY DllMain(HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH: {
		DisableThreadLibraryCalls((HMODULE)hModule);
		// Open Log File for Writing:
		char szLogFile[MAX_PATH];
		GetModuleFileName((HMODULE)hModule, szLogFile, MAX_PATH);
		strcpy(strrchr(szLogFile, (int)'.'), ".txt");
		fLogFile = fopen(szLogFile,"w");

		Real_glBegin			= (FUNC_GLBEGIN) DetourFunction((PBYTE)DetourFindFunction("OpenGl32.dll", "glBegin"),(PBYTE)Mine_glBegin);
		Real_Direct3DCreate9	= (FUNC_DIRECT3DCREATE9) DetourFunction((PBYTE)DetourFindFunction("d3d9.dll", "Direct3DCreate9"),(PBYTE)Mine_Direct3DCreate9);
		}break;
	case DLL_PROCESS_DETACH:
		DetourRemove((PBYTE)Real_glBegin,(PBYTE)Mine_glBegin);
		DetourRemove((PBYTE)Real_Direct3DCreate9,(PBYTE)Mine_Direct3DCreate9);
		if(fLogFile) fclose(fLogFile);
		break;
	default:
		break;
	}
    return TRUE;
}