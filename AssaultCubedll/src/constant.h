#pragma once
#include <cstdint>

namespace Offsets {
    inline constexpr std::uintptr_t Health = 0xEC;
    inline constexpr std::uintptr_t Armor = 0xF0;
    inline constexpr std::uintptr_t Ammo = 0x140;
    inline constexpr std::uintptr_t ViewMatrix = 0x0057DFD0;
    inline constexpr std::uintptr_t playerPositionx = 0x04;
    inline constexpr std::uintptr_t playerPositiony = 0x8;
    inline constexpr std::uintptr_t playerPositionz = 0x0C;
}

namespace Signatures {
    inline constexpr const char* LocalPlayer = "8B 0D ? ? ? ? 8B F9";
}