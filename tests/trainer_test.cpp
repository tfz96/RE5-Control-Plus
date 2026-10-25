#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include "../src/trainer_core.h"

int main()
{
    const uint8_t original[] = {0x2B, 0x44, 0x24, 0x08};
    const uint8_t applied[] = {0x90, 0x90, 0x90, 0x90};
    uint8_t bytes[4] = {};
    for (int i = 0; i < 4; ++i) bytes[i] = original[i];
    assert(trainer::State(bytes, original, applied, sizeof(bytes)) == trainer::PatchState::Original);
    for (int i = 0; i < 4; ++i) bytes[i] = applied[i];
    assert(trainer::State(bytes, original, applied, sizeof(bytes)) == trainer::PatchState::Applied);
    bytes[1] = 0xCC;
    assert(trainer::State(bytes, original, applied, sizeof(bytes)) == trainer::PatchState::Unsupported);
    puts("PASS: trainer patch state recognizes original, applied and unexpected game code.");
    return 0;
}
