#include <iostream>
#include "menu.h"
#include "Constant.h"
#include "SignatureUtils.h"
#include "dllmain.h"
#include "../include/minhook/MinHook.h"
#include <gl/GL.h>
#include "../include/imgui/imgui.h"
#include "../include/imgui/imgui_impl_win32.h"
#include "../include/imgui/imgui_impl_opengl2.h"
#include <vector>
#pragma comment(lib, "libMinHook.x86.lib")
#pragma comment(lib, "opengl32.lib")

BOOL isRunning = true;
BOOL runningHp = false;
BOOL runningArmor = false;
BOOL runningAmmo = false;
typedef BOOL(WINAPI* PFN_wglSwapBuffers)(HDC);
PFN_wglSwapBuffers oSwapBuffers = nullptr;
HWND hwnd;
bool g_ImGuiInitialized = false;
float vMatrix[4][4];
struct vec2D { float x, y; }vOut;
struct pPos { float x, y, z; } playerPos;
bool runningESP = false;
uintptr_t entityList;
uintptr_t entity;
uintptr_t LocalPlayerObject;

void infinityHp() {
    LocalPlayerObject = **(uintptr_t**)(GetAddressFromSignature("8B 0D ? ? ? ? 8B F9") + 2);
    int healthOffset = 0xEC;
    int hp = 9999999;
    *(int*)(LocalPlayerObject + healthOffset) = hp;
    ImGui::Text("Local player pointer %p", LocalPlayerObject);
}
void infinityArmor() {
    LocalPlayerObject = **(uintptr_t**)(GetAddressFromSignature("8B 0D ? ? ? ? 8B F9") + 2);
    int armorOffset = 0xF0;
    int armor = 100;
    *(int*)(LocalPlayerObject + armorOffset) = armor;
}
void infinityAmmo() {
    LocalPlayerObject = **(uintptr_t**)(GetAddressFromSignature("8B 0D ? ? ? ? 8B F9") + 2);
    int ammoOffset = 0x140;
    int ammo = 999;
    *(int*)(LocalPlayerObject + ammoOffset) = ammo;
}
uintptr_t safeToRead(uintptr_t address) {
    if (address == reinterpret_cast<uintptr_t>(nullptr)) {
        return 0;
    }

    MEMORY_BASIC_INFORMATION mbi;
    SIZE_T result = VirtualQuery(reinterpret_cast<LPCVOID>(address), &mbi, sizeof(mbi));
    if (result != sizeof(mbi)) {
        return 0;
    }

    DWORD protect = mbi.Protect;
    if (protect == 0 ||
        protect == PAGE_NOACCESS ||
        protect == PAGE_EXECUTE) {
        return 0;
    }

    uintptr_t regionStart = reinterpret_cast<uintptr_t>(mbi.BaseAddress);
    uintptr_t regionEnd = regionStart + mbi.RegionSize;
    if (address < regionStart || address >= regionEnd) {
        return 0;
    }

    return address;
}

void populateMatrix() {
    float* matrixAddr = (float*)0x0057DFD0;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            vMatrix[i][j] = matrixAddr[j * 4 + i];
        }
    }
}

bool worldToScreen(pPos& playerPos, vec2D& vOut) {

    ImVec2 screenSize = ImGui::GetIO().DisplaySize;

    float View = 0.f;
    float SightX = screenSize.x / 2, SightY = screenSize.y / 2;

    View = vMatrix[3][0] * playerPos.x + vMatrix[3][1] * playerPos.y + vMatrix[3][2] * playerPos.z + vMatrix[3][3];

    if (View <= 0.01)
        return false;

    vOut.x = SightX + (vMatrix[0][0] * playerPos.x + vMatrix[0][1] * playerPos.y + vMatrix[0][2] * playerPos.z + vMatrix[0][3]) / View * SightX;
    vOut.y = SightY - (vMatrix[1][0] * playerPos.x + vMatrix[1][1] * playerPos.y + vMatrix[1][2] * playerPos.z + vMatrix[1][3]) / View * SightY;

    return true;
}


void Cleanup() {
    MH_DisableHook(MH_ALL_HOOKS);
    MH_RemoveHook(MH_ALL_HOOKS);
    MH_Uninitialize();

    if (g_ImGuiInitialized) {
        g_ImGuiInitialized = false;
        ImGui_ImplOpenGL2_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
    }

    if (myhModule != nullptr) {
        FreeLibraryAndExitThread(myhModule, 0);
    }
    else {
        ExitThread(0);
    }
}

void ESP() {
    //entityList = **(uintptr_t**)(GetAddressFromSignature("B9 ? ? ? ? E8 ? ? ? ? 8D 44 24 ? 89 B5") + 1);
    //uintptr_t localPlayer = GetAddressFromSignature("F3 0F 59 05 ? ? ? ? 0F 5B C9 F3 0F 59 C1") + 4;
    //uintptr_t localPlayer = 0x0050F4F4;

    //ImGui::Text("Local player pointer %p", localPlayer);
    //ImGui::Text("Local player pointer %p", *(uintptr_t*)localPlayer);

    //local player pointer pattern A1 ? ? ? ? 8B 34 A8 83 3E


    //ImGui::Text("Local player probably? %p", entityList);
    //ImGui::Text("Local player 1111 %p", localPlayer);
    //ImGui::Text("Local player 2222 %p", *(uintptr_t*)localPlayer);
    /*
    for (int i{ 1 }; i != 4; i++) {
        if (safeToRead(entityList + 4*i)) {
            entity = *(uintptr_t*)(entityList + 4*i);
            playerPos.x = *(float*)(entity + 0x04);
            playerPos.y = *(float*)(entity + 0x8);
            playerPos.z = *(float*)(entity + 0x0C);
            populateMatrix();

            worldToScreen(playerPos, vOut);
            ImVec2 screenPos(vOut.x, vOut.y);
            ImGui::GetBackgroundDrawList()->AddRect(
                ImVec2(screenPos.x - 10.0f, screenPos.y - 20.0f), // botom + left
                ImVec2(screenPos.x + 10.0f, screenPos.y + 60.0f), // right + top43
                IM_COL32(255, 0, 0, 255));
        }
    }
    */
}

BOOL WINAPI hookedSwapBuffers(HDC hdc) {
    if (!g_ImGuiInitialized && hdc) {
        HWND hwnd = WindowFromDC(hdc);
        if (hwnd) {
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO();
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

            if (ImGui_ImplWin32_Init(hwnd) && ImGui_ImplOpenGL2_Init()) {
                g_ImGuiInitialized = true;
            }
            else {
                ImGui::DestroyContext();
                return oSwapBuffers(hdc);
            }
        }
        else {
            return oSwapBuffers(hdc);
        }
    }
    if (isRunning) {
        ImGui_ImplOpenGL2_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        static int selected_item = 0;
        static bool unload_cheat = false;
        const char* items[] = { "ESP", "Close", "Infinity HP", "Infinity Armor", "Infinity Ammo"};
        const int item_count = sizeof(items) / sizeof(items[0]);

        if (GetAsyncKeyState(VK_NUMPAD8) & 1) { // Up
            selected_item = (selected_item - 1 + item_count) % item_count;
        }
        else if (GetAsyncKeyState(VK_NUMPAD2) & 1) { // Down
            selected_item = (selected_item + 1) % item_count;
        }

        else if (GetAsyncKeyState(VK_NUMPAD5) & 1) {
            if (selected_item == 1) {
                isRunning = false;
                return oSwapBuffers(hdc);
            }

            else if (selected_item == 0) {
                runningESP = true;
            }
            else if (selected_item == 2) {
                runningHp = true;
            }
            else if (selected_item == 3) {
                runningArmor = true;
            }
            else if (selected_item == 4) {
                runningAmmo = true;
            }
        }

        ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
        ImGui::Begin("Cheat menu", nullptr, ImGuiWindowFlags_NoCollapse);


        for (int i = 0; i < item_count; i++) {
            if (i == selected_item) {
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "> %s", items[i]);
            }
            else {
                ImGui::Text("%s", items[i]);
            }
        }

        if (runningESP) {
            ESP();
        }
        if (runningHp) {
            infinityHp();
        }
        if (runningArmor) {
            infinityArmor();
        }
        if (runningAmmo) {
            infinityAmmo();
        }

        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
        return oSwapBuffers(hdc);
    }
}

DWORD WINAPI Menu(LPVOID) {

    if (MH_Initialize() != MH_OK) {
        MessageBoxA(nullptr, "MinHook initialization failed!", "Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    HMODULE hOpenGL = GetModuleHandleA("opengl32.dll");
    if (!hOpenGL) {
        hOpenGL = LoadLibraryA("opengl32.dll");
        if (!hOpenGL) {
            MH_Uninitialize();
            return 1;
        }
    }

    void* pSwapBuffers = GetProcAddress(hOpenGL, "wglSwapBuffers");
    if (!pSwapBuffers) {
        MH_Uninitialize();
        return 1;
    }

    if (MH_CreateHook(pSwapBuffers, hookedSwapBuffers, (void**)&oSwapBuffers) != MH_OK ||
        MH_EnableHook(pSwapBuffers) != MH_OK) {
        MH_Uninitialize();
        return 1;
    }

    while (isRunning) {
        Sleep(100);
    }

    Cleanup();

    return 0;
}