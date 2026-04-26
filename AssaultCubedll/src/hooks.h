#pragma once
#include <Windows.h>
void minHookCleanup();

typedef BOOL(WINAPI* PFN_wglSwapBuffers)(HDC);
extern PFN_wglSwapBuffers oSwapBuffers;
bool InitializeHooks();