// SignatureUtils.cpp
#include "SignatureUtils.h"
#include "Windows.h"
#include <vector>
#include <string>
#include <iostream>

using namespace std;

vector<int> idaToCodeStyle(string signature) {
    vector<int> v1;
    for (int i{ 0 }; i < signature.size(); i += 2) {
        if (signature[i] == ' ') {
            i += 1;
        }
        else if (signature[i] == '?') {
            v1.insert(v1.end(), -1);
        }
        else {
            string a = "0x" + string(1, toupper(signature[i])) + string(1, toupper(signature[i + 1]));
            v1.insert(v1.end(), stoi(a, 0, 16));
            i += 1;
        }
    }
    return v1;
}

uintptr_t GetAddressFromSignature(string sig, uintptr_t startaddress, uintptr_t endaddress) {
    vector<int> signature = idaToCodeStyle(sig);
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    if (startaddress == 0) {
        startaddress = (uintptr_t)(si.lpMinimumApplicationAddress);
    }
    if (endaddress == 0) {
        endaddress = (uintptr_t)(si.lpMaximumApplicationAddress);
    }

    MEMORY_BASIC_INFORMATION mbi{ 0 };
    DWORD protectflags = (PAGE_GUARD | PAGE_NOCACHE | PAGE_NOACCESS);

    for (uintptr_t i = startaddress; i < endaddress - signature.size(); i++) {
        if (VirtualQuery((LPCVOID)i, &mbi, sizeof(mbi))) {
            if ((mbi.Protect & protectflags) || !(mbi.State & MEM_COMMIT)) {
                i += mbi.RegionSize;
                continue;
            }

            for (uintptr_t k = (uintptr_t)mbi.BaseAddress; k < (uintptr_t)mbi.BaseAddress + mbi.RegionSize - signature.size(); k++) {
                for (size_t j = 0; j < signature.size(); j++) {
                    if (signature.at(j) != -1 && signature.at(j) != *(unsigned char*)(k + j))
                        break;
                    if (j + 1 == signature.size())
                        return k;
                }
            }
            i = (uintptr_t)mbi.BaseAddress + mbi.RegionSize;
        }
    }
    return NULL;
}
