#include <iostream>
#include "menu.h"
#include "Constant.h"
#include "SignatureUtils.h"
#include "dllmain.h"
#include <gl/GL.h>
#include "../include/imgui/imgui.h"
#include "../include/imgui/imgui_impl_win32.h"
#include "../include/imgui/imgui_impl_opengl2.h"
#include <vector>
#include "constant.h"
#include "math.h"
#include "hooks.h"
#include "features.h"
#pragma comment(lib, "opengl32.lib")

BOOL isRunning = true;
BOOL runningHp = false;
BOOL runningArmor = false;
BOOL runningAmmo = false;
HWND hwnd;
bool g_ImGuiInitialized = false;
bool runningESP = false;
uintptr_t entityList;
uintptr_t entity;

template <typename T>
bool safeRead(uintptr_t address, T& outValue) {
    if (!address) return false;

    __try {
        outValue = *(T*)address;
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        return false;
    }
}

void Cleanup() {
    minHookCleanup();

    if (myhModule != nullptr) {
        FreeLibraryAndExitThread(myhModule, 0);
    }
    else {
        ExitThread(0);
    }
}
void ESP() {
    entityList = **(uintptr_t**)(GetAddressFromSignature("B9 ? ? ? ? E8 ? ? ? ? 8D 44 24 ? 89 B5") + 1);
    populateMatrix();

    ImVec2 displaySize = ImGui::GetIO().DisplaySize;
    vec2D myScreenSize = { displaySize.x, displaySize.y };

    for (int i{ 1 }; i != 4; i++) {
        if (safeRead<uintptr_t>(entityList + 4 * i, entity)) {
            pPos playerPos;
            vec2D vOut;

            if (!safeRead<float>(entity + Offsets::playerPositionx, playerPos.x)) continue;
            if (!safeRead<float>(entity + Offsets::playerPositiony, playerPos.y)) continue;
            if (!safeRead<float>(entity + Offsets::playerPositionz, playerPos.z)) continue;

            if (worldToScreen(playerPos, vOut, myScreenSize)) {
                ImVec2 screenPos(vOut.x, vOut.y);
                ImGui::GetBackgroundDrawList()->AddRect(
                    ImVec2(screenPos.x - 10.0f, screenPos.y - 20.0f), // botom + left
                    ImVec2(screenPos.x + 10.0f, screenPos.y + 60.0f), // right + top43
                    IM_COL32(255, 0, 0, 255));
            }
        }
    }
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

                if (g_ImGuiInitialized) {
                    g_ImGuiInitialized = false;
                    ImGui_ImplOpenGL2_Shutdown();
                    ImGui_ImplWin32_Shutdown();
                    ImGui::DestroyContext();
                }

                return oSwapBuffers(hdc);
            }

            else if (selected_item == 0) {
                runningESP = !runningESP;
            }
            else if (selected_item == 2) {
                runningHp = !runningHp;
            }
            else if (selected_item == 3) {
                runningArmor = !runningArmor;
            }
            else if (selected_item == 4) {
                runningAmmo = !runningAmmo;
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
    if (!InitializeHooks()) {
        minHookCleanup();
        FreeLibraryAndExitThread(myhModule, 1);
        return 1;
    }

    while (isRunning) {
        Sleep(100);
    }

    Cleanup();

    return 0;
}