#pragma once
#include <stddef.h>
#include <stdint.h>

namespace trainer {
enum class PatchState { Original, Applied, Unsupported };

inline PatchState State(const uint8_t* bytes, const uint8_t* original, const uint8_t* applied, size_t size)
{
    bool is_original = true, is_applied = true;
    for (size_t i = 0; i < size; ++i) {
        is_original &= bytes[i] == original[i];
        is_applied &= bytes[i] == applied[i];
    }
    return is_original ? PatchState::Original : is_applied ? PatchState::Applied : PatchState::Unsupported;
}
}  // namespace trainer
