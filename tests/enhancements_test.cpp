#include <assert.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <initializer_list>
#include "../src/enhancements.cpp"

static Config testConfig;
const Config& Cfg() { return testConfig; }
void Log(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    puts("");
}

static void TestSettings() {
    using namespace enhancements;
    assert(ParseSetting("144", 0) == 144);
    assert(ParseSetting(" 144\t", 0) == 144);
    for (const char* value : {"", "144fps", "144.5", "-144", "nan", "inf", "0x90",
                              "99999999999999999999999999999999"})
        assert(ParseSetting(value, 0) == 0);
    assert(FpsLimit(0) == 0);
    assert(FpsLimit(30) == 30);
    assert(FpsLimit(120) == 120);
    assert(FpsLimit(144) == 144);
    assert(FpsLimit(1000) == 1000);
    for (int value : {-1, 1, 29, 1001}) assert(FpsLimit(value) == 0);
    assert(Config{}.fpsLimit == 144);
}

static void TestScanner() {
    using namespace enhancements;
    uint8_t bytes[64] = {};
    Match m;
    Scan(bytes, sizeof(bytes), kFpsPattern, kFpsMask, sizeof(kFpsPattern), m);
    assert(m.count == 0);
    // Include the final valid start offset and nonzero wildcard values.
    uint8_t* last = bytes + sizeof(bytes) - sizeof(kFpsPattern);
    memcpy(last, kFpsPattern, sizeof(kFpsPattern));
    last[10] = 0xFF;
    Scan(bytes, sizeof(bytes), kFpsPattern, kFpsMask, sizeof(kFpsPattern), m);
    assert(m.count == 1 && m.address == last);
    Match shortMatch;
    Scan(bytes, 2, kFpsPattern, kFpsMask, sizeof(kFpsPattern), shortMatch);
    assert(shortMatch.count == 0);
    memcpy(bytes, kFpsPattern, sizeof(kFpsPattern));
    Match duplicate;
    Scan(bytes, sizeof(bytes), kFpsPattern, kFpsMask, sizeof(kFpsPattern), duplicate);
    assert(duplicate.count == 2);
    void* original = nullptr;
    assert(!QueueHook(duplicate, (void*)FpsHook, &original, "duplicate test"));
    assert(!original);
    Match absent;
    assert(!QueueHook(absent, (void*)FpsHook, &original, "absent test"));
    // Changing the operand to a different source register must not match.
    bytes[3] = 0x4F;
    last[4] = 0x40;
    Match wrongInstruction;
    Scan(bytes, sizeof(bytes), kFpsPattern, kFpsMask, sizeof(kFpsPattern), wrongInstruction);
    assert(wrongInstruction.count == 0);
}

static void TestLiveX86Hook() {
    // A real executable x86 fixture recreates the game's SSE instruction sequence.
    // Arguments: a 64-byte fake sFps object and the numerator in xmm1.
    const BYTE fixture[] = {
        0x56,                                      // push esi
        0x8B, 0x74, 0x24, 0x08,                    // mov esi,[esp+8]
        0xF3, 0x0F, 0x10, 0x4C, 0x24, 0x0C,        // movss xmm1,[esp+12]
        0xF9,                                      // stc (the hook must preserve CF)
        0xF3, 0x0F, 0x5E, 0x4E, 0x3C,              // divss xmm1,[esi+3c]
        0x0F, 0x28, 0xC1,                          // movaps xmm0,xmm1
        0xF3, 0x0F, 0x11, 0x06,                    // movss [esi],xmm0
        0x9C, 0x58,                                // pushfd; pop eax
        0x89, 0x46, 0x04,                          // mov [esi+4],eax
        0x5E, 0xC3                                 // pop esi; ret
    };
    BYTE* code = (BYTE*)VirtualAlloc(nullptr, 4096, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    assert(code);
    memcpy(code, fixture, sizeof(fixture));
    FlushInstructionCache(GetCurrentProcess(), code, sizeof(fixture));
    using Fn = void(__cdecl*)(float*, float);
    Fn call = (Fn)code;
    float object[16] = {};
    object[15] = 120.0f;
    auto check = [&](float divisor) {
        call(object, 120.0f);
        assert(fabsf(object[0] - 120.0f / divisor) < 0.00001f);
        uint32_t flags;
        memcpy(&flags, object + 1, sizeof(flags));
        assert(flags & 1);
    };
    check(120.0f);
    assert(MH_Initialize() == MH_OK);
    enhancements::Match site{code + 12, 1};
    RE3PlusFpsResume = code + 20;
    g_fpsReady = QueueHook(site, (void*)FpsHook, &RE3PlusFpsOriginal, "x86 fixture");
    assert(g_fpsReady && RE3PlusFpsOriginal);
    UpdateEnhancementSettings();
    assert(MH_ApplyQueued() == MH_OK);
    check(144.0f);
    testConfig.fpsLimit = 60;
    UpdateEnhancementSettings();
    check(60.0f);
    testConfig.fpsLimit = 0;
    UpdateEnhancementSettings();
    check(120.0f);
    testConfig.fpsLimit = 144;
    testConfig.enabled = false;
    UpdateEnhancementSettings();
    check(120.0f);
    testConfig.enabled = true;
    UpdateEnhancementSettings();
    check(144.0f);
    g_fpsReady = false;
    UpdateEnhancementSettings();
    check(120.0f);
    assert(MH_DisableHook(code + 12) == MH_OK);
    assert(MH_Uninitialize() == MH_OK);
    check(120.0f);
    assert(VirtualFree(code, 0, MEM_RELEASE));
}

int main() {
    static_assert(sizeof(void*) == 4, "These hooks must be tested as x86");
    TestSettings();
    TestScanner();
    TestLiveX86Hook();
    puts("PASS: setting validation, unique signature scanning, live x86 hook, flags and original-path restoration.");
    return 0;
}
