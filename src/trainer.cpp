// Configuration-only versions of the requested trainer features. The instruction
// signatures were cross-checked against published RE5 1.2.0 cheat tables. Each one
// is validated against the live Steam executable before it may be changed.
#include "trainer.h"
#include "common.h"
#include "config.h"
#include "re5.h"
#include "trainer_core.h"
#include "enhancements_core.h"
#include <string.h>

using namespace re5;

namespace {
struct CodePatch {
    const char* name;
    uint32_t address;
    const uint8_t* original;
    const uint8_t* applied;
    size_t size;
    bool resolved = false;
    int last = -1;
    bool owned = false;
};

// Magazine decrement, reserve-ammo decrement and grenade stack decrement are
// runtime addresses observed after this Steam 1.2.0 executable decrypts itself.
// The disk image is SteamStub-encrypted, so these must never be inferred from
// raw file offsets.
constexpr uint8_t kMagazineOriginal[] = {0x2B, 0x44, 0x24, 0x08, 0x89, 0x41, 0x08};
constexpr uint8_t kMagazineApplied[]  = {0x90, 0x90, 0x90, 0x90, 0x89, 0x41, 0x08};
constexpr uint8_t kMagazinePattern[] = {0x8B,0x41,0x08,0x2B,0x44,0x24,0x08,0x89,0x41,0x08,0x33,0xC0,0xC2,0x08,0x00};
constexpr char kMagazineMask[] = "xxxxxxxxxxxxxxx";
constexpr uint8_t kReserveOriginal[]  = {0x29, 0x6E, 0x08, 0x5F, 0x5E};
constexpr uint8_t kReservePattern[] = {0xE9,0,0,0,0,0x29,0x6E,0x08,0x5F,0x5E,0x5D,0x33,0xC0};
constexpr char kReserveMask[] = "x????xxxxxxxx";
constexpr uint8_t kReserveApplied[]   = {0x90, 0x90, 0x90, 0x5F, 0x5E};
constexpr uint8_t kGrenadeOriginal[]  = {0x75, 0x1C, 0x8B, 0x44, 0x24, 0x14};
constexpr uint8_t kGrenadePattern[] = {0x84,0xC0,0x75,0x1C,0x8B,0x44,0x24,0x14,0x85,0xC0,0x8B,0x4D,0x14,0x89,0x41,0x08,0x7F,0x0E};
constexpr char kGrenadeMask[] = "xxxxxxxxxxxxxxxxxx";
constexpr uint8_t kGrenadeApplied[]   = {0xEB, 0x1C, 0x8B, 0x44, 0x24, 0x14};

CodePatch g_magazine {"trainer no reload", 0, kMagazineOriginal, kMagazineApplied, sizeof(kMagazineOriginal)};
CodePatch g_reserve {"trainer infinite ammo", 0, kReserveOriginal, kReserveApplied, sizeof(kReserveOriginal)};
CodePatch g_grenade {"trainer infinite grenades", 0, kGrenadeOriginal, kGrenadeApplied, sizeof(kGrenadeOriginal)};

enhancements::Match FindCode(const uint8_t* pattern, const char* mask, size_t length)
{
    enhancements::Match result;
    const BYTE* base = (const BYTE*)GetModuleHandleW(nullptr);
    if (!Readable(base, sizeof(IMAGE_DOS_HEADER))) return result;
    const auto* dos = (const IMAGE_DOS_HEADER*)base;
    if (dos->e_magic != IMAGE_DOS_SIGNATURE || dos->e_lfanew <= 0 || dos->e_lfanew > 0x100000) return result;
    const auto* nt = (const IMAGE_NT_HEADERS32*)(base + dos->e_lfanew);
    if (!Readable(nt, sizeof(*nt)) || nt->Signature != IMAGE_NT_SIGNATURE ||
        nt->FileHeader.Machine != IMAGE_FILE_MACHINE_I386 ||
        nt->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC) return result;
    const auto* sections = IMAGE_FIRST_SECTION(nt);
    if (!Readable(sections, sizeof(*sections) * nt->FileHeader.NumberOfSections)) return result;
    const size_t imageSize = nt->OptionalHeader.SizeOfImage;
    for (unsigned i = 0; i < nt->FileHeader.NumberOfSections; ++i) {
        const auto& section = sections[i];
        const size_t start = section.VirtualAddress, size = section.Misc.VirtualSize;
        if (!(section.Characteristics & IMAGE_SCN_MEM_EXECUTE) || start >= imageSize || size > imageSize - start) continue;
        const BYTE* p = base + start;
        const BYTE* end = p + size;
        const BYTE* span = p;
        while (p < end) {
            MEMORY_BASIC_INFORMATION mbi;
            if (!VirtualQuery(p, &mbi, sizeof(mbi))) break;
            const BYTE* next = (const BYTE*)mbi.BaseAddress + mbi.RegionSize;
            if (next > end) next = end;
            if (next <= p) break;
            const bool readable = mbi.State == MEM_COMMIT &&
                !(mbi.Protect & (PAGE_GUARD | PAGE_NOACCESS)) &&
                (mbi.Protect & (PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY));
            if (!readable) {
                enhancements::Scan(span, (size_t)(p - span), pattern, mask, length, result);
                span = next;
            }
            p = next;
        }
        enhancements::Scan(span, (size_t)(p - span), pattern, mask, length, result);
    }
    return result;
}

bool ResolvePatch(CodePatch& patch, const uint8_t* pattern, const char* mask, size_t length, size_t offset)
{
    if (patch.resolved) return patch.address != 0;
    patch.resolved = true;
    const enhancements::Match match = FindCode(pattern, mask, length);
    const uintptr_t address = reinterpret_cast<uintptr_t>(match.address);
    if (match.count != 1 || address > UINT32_MAX - offset) {
        Log("%s: signature matched %u times (need exactly 1); unavailable", patch.name, (unsigned)match.count);
        return false;
    }
    patch.address = (uint32_t)address + (uint32_t)offset;
    Log("%s: unique signature at %08x", patch.name, patch.address);
    return true;
}

uint32_t Rd(uint32_t address) { return Readable((void*)address, 4) ? *(uint32_t*)address : 0; }

void SetPatch(CodePatch& patch, bool enabled)
{
    if (patch.last == (enabled ? 1 : 0)) return;
    if (patch.address == 0) {
        if (enabled && patch.last != -2)
            Log("%s: no validated address; left off", patch.name);
        patch.last = enabled ? -2 : 0;
        return;
    }
    uint8_t* bytes = reinterpret_cast<uint8_t*>(patch.address);
    if (!Readable(bytes, patch.size)) {
        if (patch.last != -2) Log("%s: address unavailable", patch.name);
        patch.last = -2;
        return;
    }
    const trainer::PatchState state = trainer::State(bytes, patch.original, patch.applied, patch.size);
    if (state == trainer::PatchState::Unsupported) {
        if (patch.last != -2) Log("%s: unexpected game code, left off", patch.name);
        patch.last = -2;
        return;
    }
    if (enabled && state == trainer::PatchState::Applied) {
        patch.last = enabled ? 1 : 0;
        Log("%s: already patched in game memory", patch.name);
        return;
    }
    if (!enabled && state == trainer::PatchState::Original) {
        patch.last = 0;
        patch.owned = false;
        return;
    }
    // Do not undo a byte patch placed by an external trainer. This mod only
    // restores the original bytes after it was the component that enabled it.
    if (!enabled && !patch.owned) {
        patch.last = 0;
        Log("%s: already applied by another component; left unchanged", patch.name);
        return;
    }
    DWORD old = 0;
    if (!VirtualProtect(bytes, patch.size, PAGE_EXECUTE_READWRITE, &old)) {
        Log("%s: VirtualProtect failed (%lu)", patch.name, GetLastError());
        patch.last = -2;
        return;
    }
    memcpy(bytes, enabled ? patch.applied : patch.original, patch.size);
    VirtualProtect(bytes, patch.size, old, &old);
    FlushInstructionCache(GetCurrentProcess(), bytes, patch.size);
    patch.last = enabled ? 1 : 0;
    patch.owned = enabled;
    Log("%s: %s", patch.name, enabled ? "on" : "off");
}

bool ValidCharacter(uint32_t character)
{
    return character && Readable((void*)character, 0x3000) && (At<uint32_t>(character, ch::Flags) & 7) != 0;
}

void RestoreHealth(uint32_t character)
{
    if (!ValidCharacter(character)) return;
    int16_t& hp = At<int16_t>(character, ch::Hp);
    const int16_t maximum = At<int16_t>(character, ch::Hp + 2);
    // Do not revive a dead/critical character or manufacture a health value from bad data.
    if (hp > 0 && maximum > 0 && maximum < 30000 && hp < maximum) hp = maximum;
}

void RestoreConfiguredHealth()
{
    const uint32_t manager = Rd(kPlayerMgrPtr);
    if (!manager || !Readable((void*)(manager + 0x24), 4 * 4)) return;
    const Config& config = Cfg();
    for (uint32_t slot = 0; slot < 4; ++slot) {
        const uint32_t character = Rd(manager + 0x24 + slot * 4);
        if (!ValidCharacter(character)) continue;
        const uint32_t control = At<uint32_t>(character, ch::NetControl);
        // 0 = a locally controlled player, 1 = this machine's AI partner,
        // 2 = a remote network player's replica. Never write the remote copy.
        if (control == 0 && config.infiniteHealth) RestoreHealth(character);
        if (control == 1 && config.partnerInfiniteHealth) RestoreHealth(character);
    }
}
}  // namespace

void TrainerTick()
{
    static bool firstTickLogged = false;
    if (!firstTickLogged) {
        firstTickLogged = true;
        Log("trainer tick reached");
    }
    const Config& config = Cfg();
    const bool active = config.enabled && !config.meleeOnly;
    if (active && config.noReload)
        ResolvePatch(g_magazine, kMagazinePattern, kMagazineMask, sizeof(kMagazinePattern), 3);
    if (active && config.infiniteAmmo)
        ResolvePatch(g_reserve, kReservePattern, kReserveMask, sizeof(kReservePattern), 5);
    if (active && config.infiniteGrenades)
        ResolvePatch(g_grenade, kGrenadePattern, kGrenadeMask, sizeof(kGrenadePattern), 2);
    SetPatch(g_reserve, active && config.infiniteAmmo);
    SetPatch(g_magazine, active && config.noReload);
    SetPatch(g_grenade, active && config.infiniteGrenades);
    if (!active) return;
    RestoreConfiguredHealth();
}
