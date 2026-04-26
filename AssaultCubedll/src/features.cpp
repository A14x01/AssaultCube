#include <Windows.h>
#include "SignatureUtils.h"
#include "constant.h"

uintptr_t LocalPlayerObject;
uintptr_t localPlayerBase = 0;

uintptr_t GetLocalPlayer() {
    if (localPlayerBase == 0) {
        localPlayerBase = GetAddressFromSignature(Signatures::LocalPlayer);
        if (localPlayerBase == 0) return 0;
    }
    uintptr_t staticPointerAddress = *(uintptr_t*)(localPlayerBase + 2);
    if (staticPointerAddress == 0) return 0;

    uintptr_t playerObject = *(uintptr_t*)staticPointerAddress;
    return playerObject;
}

void infinityHp() {
    LocalPlayerObject = GetLocalPlayer();
    int newHp = 999;
    *(int*)(LocalPlayerObject + Offsets::Health) = newHp;
}
void infinityArmor() {
    LocalPlayerObject = GetLocalPlayer();
    int newArmor = 100;
    *(int*)(LocalPlayerObject + Offsets::Armor) = newArmor;
}
void infinityAmmo() {
    LocalPlayerObject = GetLocalPlayer();
    int newAmmo = 999;
    *(int*)(LocalPlayerObject + Offsets::Ammo) = newAmmo;
}