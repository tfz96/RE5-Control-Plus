// FPS hook adapted from Lyall's MIT-licensed RE5Fix.
// Keep RE3Controls' FOV and colour correction paths as the only owners of those features.
#include "common.h"
#include "config.h"
#include "enhancements.h"
#include "enhancements_core.h"
#include "MinHook.h"

// Naked x86 hooks reference C symbols directly and preserve the incoming flags and registers.
extern "C" {
alignas(4) __attribute__((used)) volatile LONG RE3PlusFpsBits = 0;
__attribute__((used)) void* RE3PlusFpsOriginal = nullptr;
__attribute__((used)) void* RE3PlusFpsResume = nullptr;
}

namespace {

bool g_fpsReady = false;

__attribute__((naked)) void FpsHook() {
    __asm__ volatile(
        "pushfl\n"
        "cmpl $0, _RE3PlusFpsBits\n"
        "je 1f\n"
        "popfl\n"
        "divss _RE3PlusFpsBits, %xmm1\n"
        "movaps %xmm1, %xmm0\n"
        "jmp *_RE3PlusFpsResume\n"
        "1: popfl\n"
        "jmp *_RE3PlusFpsOriginal\n");
}

// Scan only committed, readable executable sections of the game's mapped PE image.
// Duplicate matches are rejected across all sections instead of choosing the first one.
enhancements::Match FindCode(const uint8_t* pattern, const char* mask, size_t length) {
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
        const auto& s = sections[i];
        const size_t start = s.VirtualAddress, size = s.Misc.VirtualSize;
        if (!(s.Characteristics & IMAGE_SCN_MEM_EXECUTE) || start >= imageSize || size > imageSize - start) continue;
        const BYTE* p = base + start;
        const BYTE* end = p + size;
        // VirtualQuery may split a code section into several protection regions.
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

bool QueueHook(const enhancements::Match& match, void* hook, void** original, const char* name) {
    if (match.count != 1) {
        Log("RE5Plus %s: %u signature matches (need exactly 1); feature unavailable",
            name, (unsigned)match.count);
        return false;
    }
    void* target = (void*)match.address;
    MH_STATUS status = MH_CreateHook(target, hook, original);
    if (status == MH_OK) {
        status = MH_QueueEnableHook(target);
        if (status != MH_OK) MH_RemoveHook(target);
    }
    if (status != MH_OK) {
        Log("RE5Plus %s: MinHook failed (%d); feature unavailable", name, (int)status);
        return false;
    }
    Log("RE5Plus %s: validated hook at %p", name, target);
    return true;
}

}  // namespace

void UpdateEnhancementSettings() {
    const Config& c = Cfg();
    const int fps = c.enabled && g_fpsReady ? enhancements::FpsLimit(c.fpsLimit) : 0;
    const float divisor = (float)fps;
    LONG bits = 0;
    if (fps) memcpy(&bits, &divisor, sizeof(bits));
    const LONG oldFps = InterlockedExchange(&RE3PlusFpsBits, bits);
    if (oldFps != bits) Log("RE5Plus FPS: %d (0 = original limiter)", fps);
}

void InitEnhancementHooks() {
    using namespace enhancements;
    const Match fps = FindCode(kFpsPattern, kFpsMask, sizeof(kFpsPattern));
    if (fps.count == 1) RE3PlusFpsResume = (void*)(fps.address + 8);
    g_fpsReady = QueueHook(fps, (void*)FpsHook, &RE3PlusFpsOriginal, "FPS limiter");
    UpdateEnhancementSettings();
}
