#pragma once

#include "Windows.h"
#include <vector>
#include <string>

uintptr_t GetAddressFromSignature(std::string sig, uintptr_t startaddress = 0, uintptr_t endaddress = 0);
std::vector<int> idaToCodeStyle(std::string signature);
