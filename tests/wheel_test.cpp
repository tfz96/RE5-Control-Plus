#include <assert.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <initializer_list>
#include "../src/common.h"
#include "../src/re5.h"

// Redirect fixed game data addresses into fixture storage; the hook instructions execute natively.
static uint32_t testGlobals[6];
static BYTE testWeaponInfo[256 * 0x140];
static uint32_t Translate(uint32_t address) {
    const uint32_t globals[] = {0x01249C40, 0x01249C78, 0x01249C74, 0x012340A4, 0x0123457C, 0x011B27B4};
    for (int i = 0; i < 6; ++i)
        if (address == globals[i]) return (uint32_t)(uintptr_t)&testGlobals[i];
    if (address >= 0x011B5170 && address < 0x011B5170 + sizeof(testWeaponInfo))
        return (uint32_t)(uintptr_t)testWeaponInfo + address - 0x011B5170;
    return address;
}
static bool TestReadable(const void* p, size_t n) {
    return Readable((void*)Translate((uint32_t)(uintptr_t)p), n);
}
namespace re5 {
template <class T> T& TestAt(uint32_t base, uint32_t offset) {
    return At<T>(Translate(base + offset), 0);
}
}
#define Readable TestReadable
#define At TestAt
#include "../src/kbinput.cpp"
#undef At
#undef Readable

static Config testConfig;
const Config& Cfg() { return testConfig; }
void Log(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    puts("");
}

static void TestSelection() {
    using namespace wheel;
    Item items[] = {{10, 8, false, true}, {20, 4, true, true}, {30, 0, false, true},
                    {40, 2, false, false}, {0, 6, false, true}, {60, 10, false, true}};
    assert(Select(items, 6, -120) == 10);
    assert(Select(items, 6, 120) == 30);
    assert(Select(items, 6, 0) == 0);
    items[1].equipped = false;
    items[0].equipped = true;
    assert(Select(items, 6, -1) == 30);
    items[0].equipped = false;
    items[2].equipped = true;
    assert(Select(items, 6, 1) == 10);
    items[2].equipped = false;
    items[3].equipped = true;  // A non-gun in hand still anchors grid order.
    assert(Select(items, 6, -1) == 20);
    assert(Select(items, 6, 1) == 30);
    items[3].equipped = false;
    assert(Select(items, 6, -1) == 30);
    assert(Select(items, 6, 1) == 10);
    const Item one{10, 8, true, true};
    assert(Select(&one, 1, -1) == 0 && Select(&one, 1, 1) == 0);
    assert(Select(nullptr, 0, 1) == 0);
    const Item empty{10, 2, true, false};
    assert(Select(&empty, 1, 1) == 0);
    for (uint32_t group : {4u, 8u, 16u, 64u, 128u, 256u, 512u, 4194304u}) {
        assert(Gun(0x101, group));
        assert(!Gun(0x200, group));
    }
    assert(Gun(0x115, 0));
    assert(!Gun(0x101, 0));
    assert(!Gun(0x101, 32));
    for (uint32_t vt : {plact::Relax, plact::Run, plact::ReadyLoop, plact::ReadyEnd})
        assert(Stance(vt, 1));
    for (uint32_t vt : {0u, plact::Reload, plact::Fire, plact::WeaponChange, plact::WeaponChangeReady})
        assert(!Stance(vt, 1));
    assert(!Stance(plact::ReadyLoop, aid::KnifeLoop));
    Cooldown cooldown;
    assert(cooldown.Ready(100, 0));
    cooldown.Commit(100, 0xFFFFFFF0);
    assert(!cooldown.Ready(100, 0xFFFFFFF1));
    assert(!cooldown.Ready(100, 0x85));
    assert(cooldown.Ready(100, 0x86));  // Tick rollover, exactly 150 ms.
    assert(cooldown.Ready(200, 1));
}

struct Fixture {
    BYTE character[0x3000], spad[0x620], mouse[0xA0], keyboard[0x180], grid[0x80];
    BYTE action[4], main[0x10600], scene[0x3090], command[0x440];
};
static Fixture f;
static uint32_t Address(void* p) { return (uint32_t)(uintptr_t)p; }
static uint32_t Ch() { return Address(f.character); }
static void __fastcall OriginalSwitch(uint32_t, void*) {}
static uint32_t __fastcall ExistingItem(uint32_t grid, void*, uint32_t character, uint32_t pad) {
    assert(grid == Address(f.grid) && character == Ch() && pad == 0x12345678);
    return 2;
}

static void Reset() {
    memset(&f, 0, sizeof(f));
    testConfig = Config{};
    g_wheelCooldown = {};
    InterlockedExchange(&g_wheelDelta, 0);
    oSwitch = OriginalSwitch;
    testGlobals[0] = Address(f.spad);
    testGlobals[1] = Address(f.mouse);
    testGlobals[2] = Address(f.keyboard);
    testGlobals[3] = Address(f.main);
    testGlobals[4] = Address(f.scene);
    testGlobals[5] = 0;
    At<uint32_t>(Address(f.spad), 0x614) = 0;
    At<uint8_t>(Address(f.spad), 0x610) = 1;
    At<float>(Address(f.mouse), 0x9C) = 1;
    At<uint32_t>(Ch(), ch::Flags) = 1;
    At<int16_t>(Ch(), ch::Hp) = 100;
    At<uint32_t>(Ch(), ch::CurAction) = Address(f.action);
    At<uint32_t>(Address(f.action), 0) = plact::Relax;
    At<uint32_t>(Ch(), ch::ActionId) = 1;
    At<uint32_t>(Ch(), 0x2654) = Address(f.grid);
    for (uint32_t offset : {0x58u, 0x5Cu, 0x60u, 0x64u})
        At<uint32_t>(Address(f.grid), offset) = 0xD;
    const uint32_t inv = Ch() + wheel::kInventory;
    const uint32_t slots[] = {4, 0, 8, 2, 6};
    const uint32_t ids[] = {0x101, 0x102, 0x103, 0x200, 0x104};
    for (int i = 0; i < 5; ++i) {
        const uint32_t entry = inv + 4 + i * 0x30;
        At<uint32_t>(entry, 4) = ids[i];
        At<uint32_t>(entry, 0x18) = slots[i];
        At<uint32_t>(entry, 0x1C) = i == 0 ? 1 : 0;
        At<uint8_t>(entry, 0x29) = i == 4 ? 0 : 1;
        At<uint32_t>(Address(testWeaponInfo), (ids[i] & 0xFF) * 0x140 + 0xB8) = 4;
    }
}

using Builder = void(__cdecl*)(void*, void*, uint32_t);
static uint32_t testBuilder;
static uint32_t RunBuilder() {
    BYTE characterBefore[sizeof(f.character)], gridBefore[sizeof(f.grid)];
    memcpy(characterBefore, f.character, sizeof(characterBefore));
    memcpy(gridBefore, f.grid, sizeof(gridBefore));
    memset(f.command, 0, sizeof(f.command));
    ((Builder)(testBuilder - 15))(f.character, f.command, 0);
    const uint32_t slot = At<uint32_t>(Address(f.command), 0x430);
    BYTE expected[sizeof(f.command)] = {};
    const uint32_t commands[] = {slot, At<uint32_t>(Address(f.grid), 0x5C),
                               At<uint32_t>(Address(f.grid), 0x60), At<uint32_t>(Address(f.grid), 0x64)};
    memcpy(expected + 0x430, commands, sizeof(commands));
    expected[0x24] = expected[0x25] = slot != 0xD;
    expected[0x63] = expected[0x65] = commands[1] != 0xD;
    expected[0x67] = commands[2] != 0xD;  // GIVE, never set by scrolling.
    expected[0x61] = commands[3] != 0xD;
    assert(!memcmp(expected, f.command, sizeof(expected)));
    assert(!memcmp(characterBefore, f.character, sizeof(characterBefore)));
    assert(!memcmp(gridBefore, f.grid, sizeof(gridBefore)));
    return slot;
}
static void Capture(int delta) {
    At<int32_t>(Address(f.mouse), 0x34) = delta;
    HookDeviceSwitch(Address(f.spad), nullptr);
}
static uint32_t Request(int delta = -120) {
    Capture(delta);
    return RunBuilder();
}

static void TestRuntime() {
    // Independent transcription of the native four-command builder at B8AAB7..B8AB7C.
    // Execute equip AND give paths: the old hook passes selection tests but fails here.
    const BYTE nativeBuilder[] = {
        0x57, 0xC7, 0x83, 0x30, 0x04, 0x00, 0x00, 0x0D, 0x00, 0x00, 0x00,
        0x8B, 0x8E, 0x54, 0x26, 0x00, 0x00, 0x56, 0xE8, 0x62, 0xF0, 0x0D, 0x00,
        0x83, 0xF8, 0x0D, 0x74, 0x0E, 0x89, 0x83, 0x30, 0x04, 0x00, 0x00,
        0xC6, 0x43, 0x24, 0x01, 0xC6, 0x43, 0x25, 0x01,
        0x57, 0xC7, 0x83, 0x34, 0x04, 0x00, 0x00, 0x0D, 0x00, 0x00, 0x00,
        0x8B, 0x8E, 0x54, 0x26, 0x00, 0x00, 0x56, 0xE8, 0x68, 0xF0, 0x0D, 0x00,
        0x83, 0xF8, 0x0D, 0x74, 0x0E, 0x89, 0x83, 0x34, 0x04, 0x00, 0x00,
        0xC6, 0x43, 0x63, 0x01, 0xC6, 0x43, 0x65, 0x01,
        0x8D, 0x8E, 0xA0, 0x21, 0x00, 0x00,
        0xC7, 0x83, 0x38, 0x04, 0x00, 0x00, 0x0D, 0x00, 0x00, 0x00,
        0xE8, 0x60, 0x0F, 0x0C, 0x00, 0x85, 0xC0, 0x74, 0x0D,
        0x8B, 0x40, 0x18, 0x89, 0x83, 0x38, 0x04, 0x00, 0x00, 0xC6, 0x43, 0x67, 0x01,
        0x83, 0xBB, 0x38, 0x04, 0x00, 0x00, 0x0D, 0x75, 0x1C,
        0x8B, 0x8E, 0x54, 0x26, 0x00, 0x00, 0x57, 0x56, 0xE8, 0x49, 0xF0, 0x0D, 0x00,
        0x83, 0xF8, 0x0D, 0x74, 0x0A, 0x89, 0x83, 0x38, 0x04, 0x00, 0x00, 0xC6, 0x43, 0x67, 0x01,
        0x57, 0xC7, 0x83, 0x3C, 0x04, 0x00, 0x00, 0x0D, 0x00, 0x00, 0x00,
        0x8B, 0x8E, 0x54, 0x26, 0x00, 0x00, 0x56, 0xE8, 0x53, 0xF0, 0x0D, 0x00,
        0x83, 0xF8, 0x0D, 0x74, 0x0A, 0x89, 0x83, 0x3C, 0x04, 0x00, 0x00, 0xC6, 0x43, 0x61, 0x01
    };
    static_assert(sizeof(nativeBuilder) == 0xB8AB7C - 0xB8AAB7);
    const uint32_t code = Address(VirtualAlloc(nullptr, 0x100000, MEM_COMMIT | MEM_RESERVE | MEM_TOP_DOWN,
                                              PAGE_EXECUTE_READWRITE));
    assert(code);
    testBuilder = code + 15;
    const uint32_t query = testBuilder + 0xC69B30 - 0xB8AAB7;
    const BYTE prologue[] = {
        0x56, 0x53, 0x57,                  // preserve esi, ebx, edi
        0x8B, 0x74, 0x24, 0x10,            // esi = character
        0x8B, 0x5C, 0x24, 0x14,            // ebx = command
        0x8B, 0x7C, 0x24, 0x18             // edi = pad
    };
    memcpy((void*)code, prologue, sizeof(prologue));
    memcpy((void*)testBuilder, nativeBuilder, sizeof(nativeBuilder));
    const BYTE epilogue[] = {0x5F, 0x5B, 0x5E, 0xC3};
    memcpy((void*)(testBuilder + sizeof(nativeBuilder)), epilogue, sizeof(epilogue));
    for (uint32_t i = 0; i < 4; ++i) {
        BYTE nativeQuery[] = {0x8B, 0x41, (BYTE)(0x58 + i * 4), 0xC2, 0x08, 0x00, 0xCC, 0xCC};
        memcpy((void*)(query + i * 0x30), nativeQuery, sizeof(nativeQuery));
    }
    const BYTE unusedGiveQuery[] = {0x33, 0xC0, 0xC3, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC};
    const uint32_t oldQuery = testBuilder + 0xC4BA80 - 0xB8AAB7;
    memcpy((void*)oldQuery, unusedGiveQuery, sizeof(unusedGiveQuery));
    FlushInstructionCache(GetCurrentProcess(), nullptr, 0);
    assert(MH_Initialize() == MH_OK);
    assert(!InitWheelHook(false, query, testBuilder));
    *(BYTE*)testBuilder ^= 1;
    assert(!InitWheelHook(true, query, testBuilder));
    *(BYTE*)testBuilder ^= 1;
    *(BYTE*)query ^= 1;
    assert(!InitWheelHook(true, query, testBuilder));
    *(BYTE*)query ^= 1;
    assert(InitWheelHook(true, query, testBuilder));
    assert(MH_ApplyQueued() == MH_OK);
    assert(!memcmp((void*)oldQuery, unusedGiveQuery, sizeof(unusedGiveQuery)));

    Reset();
    BYTE inventoryBefore[4 + 24 * 0x30];
    memcpy(inventoryBefore, f.character + wheel::kInventory, sizeof(inventoryBefore));
    assert(Request() == 8);                 // Order is 0,4,8, not internal array order.
    assert(RunBuilder() == 0xD);            // Same frame cannot consume the wheel twice.
    assert(!memcmp(inventoryBefore, f.character + wheel::kInventory, sizeof(inventoryBefore)));
    Reset();
    assert(Request(120) == 0);
    Reset();
    Capture(-120);
    assert(((PFN_WheelQuery)query)(Address(f.grid), nullptr, Ch(), 0) == 0xD);
    assert(RunBuilder() == 8);              // Other callers do not receive or consume a request.
    Reset();
    PFN_WheelQuery saved = oWheelQuery;
    oWheelQuery = ExistingItem;
    assert(HookWheelQuery(Address(f.grid), nullptr, Ch(), 0x12345678) == 2);
    oWheelQuery = saved;

    // Native UI actions keep their own channels; scrolling must never add a second action.
    for (uint32_t offset : {0x58u, 0x5Cu, 0x60u, 0x64u}) {
        Reset();
        At<uint32_t>(Address(f.grid), offset) = 2;
        assert(Request() == (offset == 0x58 ? 2 : 0xD));
        At<uint32_t>(Address(f.grid), offset) = 0xD;
        assert(RunBuilder() == 0xD);  // The rejected scroll is discarded.
    }
    // Repeated cycles retain all items and never request the empty-handed slot.
    for (int direction : {-120, 120}) {
        Reset();
        const uint32_t sequenceDown[] = {8, 0, 4}, sequenceUp[] = {0, 8, 4};
        for (int n = 0; n < 30; ++n) {
            g_wheelCooldown.tick = GetTickCount() - 150;
            const uint32_t slot = Request(direction);
            assert(slot == (direction < 0 ? sequenceDown : sequenceUp)[n % 3]);
            for (int i = 0; i < 5; ++i) {
                const uint32_t entry = Ch() + wheel::kInventory + 4 + i * 0x30;
                At<uint32_t>(entry, 0x1C) = At<uint32_t>(entry, 0x18) == slot ? 1 : 0;
            }
        }
    }
    Reset();
    for (int i : {1, 2}) f.character[wheel::kInventory + 4 + i * 0x30 + 0x29] = 0;
    assert(Request() == 0xD);  // Only the currently equipped gun remains.

    // Each blocked request is discarded, including after the blocking state ends.
    auto blocked = [&](auto set, auto clear) {
        Reset();
        set();
        assert(Request() == 0xD);
        clear();
        Capture(0);
        assert(RunBuilder() == 0xD);
        assert(Request() == 8);
    };
    blocked([] { testConfig.enabled = false; }, [] { testConfig.enabled = true; });
    blocked([] { testConfig.keyboardMouse = false; }, [] { testConfig.keyboardMouse = true; });
    blocked([] { testConfig.mouseWheelWeapons = false; }, [] { testConfig.mouseWheelWeapons = true; });
    blocked([] { testConfig.meleeOnly = true; }, [] { testConfig.meleeOnly = false; });
    blocked([] { f.spad[spad::InputLocked] = 1; }, [] { f.spad[spad::InputLocked] = 0; });
    blocked([] { f.spad[spad::DeviceIsPad] = 1; }, [] { f.spad[spad::DeviceIsPad] = 0; });
    blocked([] { f.spad[0x610] = 0; }, [] { f.spad[0x610] = 1; });
    blocked([] { f.main[0x1054C] = 1; }, [] { f.main[0x1054C] = 0; });
    blocked([] { f.scene[0x3085] = 1; }, [] { f.scene[0x3085] = 0; });
    blocked([] { f.character[ch::Scope] = 1; }, [] { f.character[ch::Scope] = 0; });
    blocked([] { At<int16_t>(Ch(), ch::Hp) = 0; }, [] { At<int16_t>(Ch(), ch::Hp) = 100; });
    blocked([] { At<uint32_t>(Ch(), ch::NetControl) = 1; }, [] { At<uint32_t>(Ch(), ch::NetControl) = 0; });
    blocked([] { At<uint32_t>(Ch(), ch::NetControl) = 2; }, [] { At<uint32_t>(Ch(), ch::NetControl) = 0; });
    blocked([] { At<uint32_t>(Address(f.spad), 0x614) = 1; },
            [] { At<uint32_t>(Address(f.spad), 0x614) = 0; });
    blocked([] { f.grid[0x68] = 1; }, [] { f.grid[0x68] = 0; });
    blocked([] { At<uint32_t>(Address(f.grid), 0x60) = 2; },
            [] { At<uint32_t>(Address(f.grid), 0x60) = 0xD; });
    blocked([] { At<uint32_t>(Ch(), 0x2D8C) = 0x4080; }, [] { At<uint32_t>(Ch(), 0x2D8C) = 0; });
    blocked([] { At<uint32_t>(Ch(), 0x2D7C) = 0x80000000; }, [] { At<uint32_t>(Ch(), 0x2D7C) = 0; });
    blocked([] { At<float>(Ch(), 0x265C) = 1; }, [] { At<float>(Ch(), 0x265C) = 0; });
    blocked([] { f.character[0x1BCC] = 1; }, [] { f.character[0x1BCC] = 0; });
    for (uint32_t vt : {plact::Reload, plact::WeaponChange, plact::WeaponChangeReady, plact::Fire}) {
        blocked([&] { At<uint32_t>(Address(f.action), 0) = vt; },
                [] { At<uint32_t>(Address(f.action), 0) = plact::Relax; });
    }
    Reset();
    At<uint32_t>(Address(f.action), 0) = plact::ReadyLoop;
    assert(Request() == 8);  // Normal aiming works; a scope above remains blocked.
    Reset();
    Capture(-120);
    Capture(0);             // A frame without a character update cannot leave buffered input.
    assert(RunBuilder() == 0xD);
    assert(WheelItem(1) == 0);
    assert(MH_DisableHook((void*)query) == MH_OK);
    Reset();
    assert(Request() == 0xD);
    assert(MH_Uninitialize() == MH_OK);
    assert(VirtualFree((void*)code, 0, MEM_RELEASE));
}

int main() {
    static_assert(sizeof(void*) == 4, "Test the actual x86 calling convention");
    assert(Config{}.mouseWheelWeapons);
    TestSelection();
    TestRuntime();
    puts("PASS: wheel order, wrap, cooldown, native x86 equip/give separation, unchanged inventory, UI priority and input gates.");
}
