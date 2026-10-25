#pragma once
#include <stddef.h>
#include <stdint.h>
#include "re5.h"

namespace wheel {

constexpr uint32_t kQuery = 0x00C69B30;
constexpr uint32_t kBuilder = 0x00B8AAB7;
constexpr uint32_t kReturn = 0x00B8AACE;
constexpr uint32_t kInventory = 0x21A0;
constexpr uint8_t kQueryBytes[] = {0x8B, 0x41, 0x58, 0xC2, 0x08, 0x00, 0xCC, 0xCC};
constexpr uint8_t kBuilderBytes[] = {
    0x57,                                           // pad argument
    0xC7, 0x83, 0x30, 0x04, 0x00, 0x00, 0x0D, 0x00, 0x00, 0x00,
    0x8B, 0x8E, 0x54, 0x26, 0x00, 0x00,             // inventory UI in ecx
    0x56,                                           // character argument
    0xE8, 0x62, 0xF0, 0x0D, 0x00,                   // query returns slot or 0xD
    0x83, 0xF8, 0x0D, 0x74, 0x0E,
    0x89, 0x83, 0x30, 0x04, 0x00, 0x00,
    0xC6, 0x43, 0x24, 0x01, 0xC6, 0x43, 0x25, 0x01 // equip flags; +0x67 is GIVE
};

inline bool Gun(uint32_t id, uint32_t group) {
    if ((id & 0xFF00) != 0x100) return false;
    if (id == 0x115) return true;  // Gatling gun has group 0.
    switch (group) {
        case 4: case 8: case 16: case 64: case 128: case 256: case 512: case 4194304:
            return true;
    }
    return false;
}

inline bool Stance(uint32_t vtable, uint32_t actionId) {
    using namespace re5;
    if (actionId >= aid::KnifeBegin && actionId <= aid::KnifeAttack) return false;
    switch (vtable) {
        case plact::Relax: case plact::Wait: case plact::Walk: case plact::WalkL: case plact::WalkR:
        case plact::Run: case plact::Back: case plact::TurnL: case plact::TurnR:
        case plact::ReadyLoop: case plact::ReadyEnd:
            return true;
    }
    return false;
}

struct Item {
    uint32_t address = 0;
    uint32_t slot = 0;
    bool equipped = false;
    bool gun = false;
};

// Select by visible grid order, independent of the inventory's internal array order.
inline uint32_t Select(const Item* items, size_t count, int delta) {
    if (!delta) return 0;
    uint32_t slots[9] = {};
    int current = -1;
    for (size_t i = 0; i < count; ++i) {
        const Item& item = items[i];
        if (!item.address || item.slot >= 9) continue;
        if (item.equipped) current = (int)item.slot;
        if (item.gun) slots[item.slot] = item.address;
    }
    const int step = delta < 0 ? 1 : -1;  // Down = next, up = previous.
    if (current < 0) current = step > 0 ? -1 : 9;
    for (int n = 1; n <= 9; ++n) {
        const int slot = (current + step * n + 18) % 9;
        if (slots[slot] && slot != current) return slots[slot];
    }
    return 0;
}

struct Cooldown {
    uint32_t character = 0;
    uint32_t tick = 0;
    bool used = false;

    bool Ready(uint32_t owner, uint32_t now) const {
        return !used || character != owner || (uint32_t)(now - tick) >= 150;
    }
    void Commit(uint32_t owner, uint32_t now) {
        character = owner;
        tick = now;
        used = true;
    }
};

}  // namespace wheel
