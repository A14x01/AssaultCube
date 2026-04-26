#include "hooks.h"
#include "menu.h"
#include "../include/minhook/MinHook.h"
#pragma comment(lib, "libMinHook.x86.lib")

void minHookCleanup() {
	MH_DisableHook(MH_ALL_HOOKS);
	MH_RemoveHook(MH_ALL_HOOKS);
	MH_Uninitialize();
}

PFN_wglSwapBuffers oSwapBuffers = nullptr;

bool InitializeHooks() {
    if (MH_Initialize() != MH_OK) {
        MessageBoxA(nullptr, "MinHook initialization failed!", "Error", MB_OK | MB_ICONERROR);
        return false;
    }

    HMODULE hOpenGL = GetModuleHandleA("opengl32.dll");
    if (!hOpenGL) {
        hOpenGL = LoadLibraryA("opengl32.dll");
        if (!hOpenGL) return false;
    }

    void* pSwapBuffers = GetProcAddress(hOpenGL, "wglSwapBuffers");
    if (!pSwapBuffers) return false;

    if (MH_CreateHook(pSwapBuffers, hookedSwapBuffers, (void**)&oSwapBuffers) != MH_OK ||
        MH_EnableHook(pSwapBuffers) != MH_OK) {
        return false;
    }

    return true;
}
