#pragma once
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>

namespace enhancements {

inline int ParseSetting(const char* text, int fallback) {
    if (!text) return fallback;
    errno = 0;
    char* end = nullptr;
    const long value = strtol(text, &end, 10);
    if (end == text || errno == ERANGE) return fallback;
    while (*end == ' ' || *end == '\t') ++end;
    if (*end || value < 0 || value > 10000) return fallback;
    return (int)value;
}

inline int FpsLimit(int value) {
    // 0 restores the original limiter.
    if (value == 0) return 0;
    return value >= 30 && value <= 1000 ? value : 0;
}

// These include the exact overwritten instructions, unlike RE5Fix's wildcarded operands.
constexpr uint8_t kFpsPattern[] = {
    0xF3, 0x0F, 0x5E, 0x4E, 0x3C, 0x0F, 0x28, 0xC1,
    0xF3, 0x0F, 0, 0, 0xF3, 0x0F, 0, 0, 0, 0, 0xD9, 0x54
};
constexpr char kFpsMask[] = "xxxxxxxxxx??xx????xx";
static_assert(sizeof(kFpsPattern) == sizeof(kFpsMask) - 1, "FPS signature size");

struct Match {
    const uint8_t* address = nullptr;
    size_t count = 0;
};

inline void Scan(const uint8_t* data, size_t size, const uint8_t* pattern, const char* mask,
                 size_t length, Match& result) {
    if (!data || !pattern || !mask || !length || size < length) return;
    for (size_t i = 0; i <= size - length; ++i) {
        if (data[i] != pattern[0]) continue;
        size_t j = 1;
        for (; j < length; ++j)
            if (mask[j] == 'x' && data[i + j] != pattern[j]) break;
        if (j == length) {
            if (!result.count) result.address = data + i;
            ++result.count;
        }
    }
}

}  // namespace enhancements
