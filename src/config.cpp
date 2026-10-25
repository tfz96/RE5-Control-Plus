#include "common.h"
#include "config.h"
#include "enhancements_core.h"
#include <stdio.h>
#include <stdlib.h>

static Config g_cfg;

const Config& Cfg() { return g_cfg; }

static const char kDefaultIni[] =
#include "default_ini.inc"
;

static bool GetBool(const char* path, const char* sec, const char* key, bool def) {
    return GetPrivateProfileIntA(sec, key, def ? 1 : 0, path) != 0;
}

static float GetFloat(const char* path, const char* sec, const char* key, float def) {
    char buf[64], dflt[64];
    sprintf(dflt, "%g", def);
    GetPrivateProfileStringA(sec, key, dflt, buf, sizeof(buf), path);
    return (float)atof(buf);
}

// A whole number like "0.5" or "0,5" (spaces around allowed); false for empty or other text.
static bool ParseNumber(char* buf, float* out) {
    for (char* p = buf; *p; ++p)
        if (*p == ',') *p = '.';
    char* end = nullptr;
    const double v = strtod(buf, &end);
    if (end == buf) return false;
    while (*end == ' ' || *end == '\t') ++end;
    if (*end) return false;
    *out = (float)v;
    return v == v;
}

// A speed multiplier: "1,5" works like "1.5"; anything that is not a positive number means the default.
static float GetScale(const char* path, const char* sec, const char* key, float lo, float hi, float def = 1.0f) {
    char buf[64], dflt[32];
    sprintf(dflt, "%g", def);
    GetPrivateProfileStringA(sec, key, dflt, buf, sizeof(buf), path);
    for (char* p = buf; *p; ++p)
        if (*p == ',') *p = '.';
    const float v = (float)atof(buf);
    if (!(v > 0.0f)) return def;  // also NaN
    return v < lo ? lo : v > hi ? hi : v;
}

static FILETIME g_iniTime = {};

// Re-read the ini when it changes on disk (cheap: one attribute query per second).
void PollConfig() {
    static DWORD last = 0;
    DWORD now = GetTickCount();
    if (now - last < 1000) return;
    last = now;
    char path[MAX_PATH];
    wsprintfA(path, "%sRE3Controls.ini", ModDir());
    WIN32_FILE_ATTRIBUTE_DATA a;
    if (!GetFileAttributesExA(path, GetFileExInfoStandard, &a)) return;
    if (CompareFileTime(&a.ftLastWriteTime, &g_iniTime) != 0) LoadConfig();
}

void LoadConfig() {
    char path[MAX_PATH];
    wsprintfA(path, "%sRE3Controls.ini", ModDir());
    if (GetFileAttributesA(path) == INVALID_FILE_ATTRIBUTES) {
        HANDLE h = CreateFileA(path, GENERIC_WRITE, 0, nullptr, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (h != INVALID_HANDLE_VALUE) {
            DWORD w;
            WriteFile(h, kDefaultIni, sizeof(kDefaultIni) - 1, &w, nullptr);
            CloseHandle(h);
        }
    }
    WIN32_FILE_ATTRIBUTE_DATA fa;
    if (GetFileAttributesExA(path, GetFileExInfoStandard, &fa)) g_iniTime = fa.ftLastWriteTime;
    // The profile API keeps the last ini file it read cached; drop it so edits made while the game runs apply.
    WritePrivateProfileStringA(nullptr, nullptr, nullptr, path);
    static bool logged = false;
    if (!logged) {
        Log("config file: %s", path);
        logged = true;
    }
    Config c;
    c.enabled = GetBool(path, "Controls", "Enabled", c.enabled);
    c.remapButtons = GetBool(path, "Controls", "RemapButtons", c.remapButtons);
    char run[32];
    GetPrivateProfileStringA("Controls", "Run", "Toggle", run, sizeof(run), path);
    c.runMode = !_stricmp(run, "Hold") ? RunMode::Hold : !_stricmp(run, "Stock") ? RunMode::Stock : RunMode::Toggle;
    char runButton[32];
    GetPrivateProfileStringA("Controls", "RunButton", "L3", runButton, sizeof(runButton), path);
    c.runButton = !_stricmp(runButton, "RB") ? RunButton::RB : RunButton::L3;
    c.oneButtonReload = GetBool(path, "Controls", "OneButtonReload", c.oneButtonReload);
    c.quickTurn = GetBool(path, "Controls", "QuickTurn", c.quickTurn);
    c.weskerDash = GetBool(path, "Controls", "WeskerDash", c.weskerDash);
    c.stickClicks = GetBool(path, "Controls", "StickClicks", c.stickClicks);
    c.knifeFromAim = GetBool(path, "Controls", "KnifeFromAim", c.knifeFromAim);
    c.inventoryMove = GetBool(path, "Controls", "InventoryMove", c.inventoryMove);
    c.freeCamera = GetBool(path, "Camera", "FreeCamera", c.freeCamera);
    c.cameraSpeed = GetFloat(path, "Camera", "Speed", c.cameraSpeed);
    c.cameraPitchSpeed = GetFloat(path, "Camera", "PitchSpeed", c.cameraPitchSpeed);
    c.invertCameraY = GetBool(path, "Camera", "InvertY", c.invertCameraY);
    c.cameraDistance = GetFloat(path, "Camera", "Distance", c.cameraDistance);
    c.cameraHeight = GetFloat(path, "Camera", "Height", c.cameraHeight);
    c.cameraShoulder = GetFloat(path, "Camera", "Shoulder", c.cameraShoulder);
    c.cameraDefaultPitch = GetFloat(path, "Camera", "Tilt", c.cameraDefaultPitch);
    {
        char buf[64];
        GetPrivateProfileStringA("Camera", "AimDistance", "0", buf, sizeof(buf), path);
        float v = 0.0f;
        c.aimDistance = ParseNumber(buf, &v) && v > 0.0f ? (v > 200.0f ? 200.0f : v) : 0.0f;
    }
    {
        auto degrees = [&](const char* key) {
            char buf[64];
            GetPrivateProfileStringA("Camera", key, "0", buf, sizeof(buf), path);
            float v = 0.0f;
            if (!ParseNumber(buf, &v) || !(v == v)) return 0.0f;
            return v < -15.0f ? -15.0f : v > 40.0f ? 40.0f : v;
        };
        c.fieldOfView = degrees("FieldOfView");
        c.aimFieldOfView = degrees("AimFieldOfView");
    }
    c.turnSpeed = GetFloat(path, "Camera", "TurnSpeed", c.turnSpeed);
    c.backStep = GetBool(path, "Camera", "BackStep", c.backStep);
    c.coverTurn = GetBool(path, "Camera", "CoverTurn", c.coverTurn);
    c.followPlatform = GetBool(path, "Camera", "FollowPlatform", c.followPlatform);
    c.meleeCamera = GetBool(path, "Camera", "MeleeCamera", c.meleeCamera);
    c.grabCamera = GetBool(path, "Camera", "GrabCamera", c.grabCamera);
    // AimWalk (up to v1.4) is the default for the three separate switches.
    const bool aimWalk = GetBool(path, "Aim", "AimWalk", true);
    c.gunWalk = GetBool(path, "Aim", "GunWalk", aimWalk);
    c.knifeWalk = GetBool(path, "Aim", "KnifeWalk", aimWalk);
    c.reloadWalk = GetBool(path, "Aim", "ReloadWalk", aimWalk);
    c.switchWalk = GetBool(path, "Aim", "SwitchWalk", aimWalk);
    c.scopeWalk = GetBool(path, "Aim", "ScopeWalk", c.scopeWalk);
    {
        char ib[16];
        GetPrivateProfileStringA("Controls", "InteractButton", "A", ib, sizeof(ib), path);
        c.interactX = !_stricmp(ib, "X");
    }
    c.aimWalkAnimSpeed = GetFloat(path, "Aim", "WalkSpeed", c.aimWalkAnimSpeed);
    c.aimWalkFastSpeed = GetFloat(path, "Aim", "FastWalkSpeed", c.aimWalkFastSpeed);
    c.crosshair = GetBool(path, "Aim", "Crosshair", c.crosshair);
    c.directHorizontal = GetBool(path, "Aim", "DirectHorizontal", c.directHorizontal);
    c.aimSpeedX = GetScale(path, "Aim", "AimSpeedX", 0.25f, 4.0f);
    c.aimSpeedY = GetScale(path, "Aim", "AimSpeedY", 0.25f, 4.0f);
    {
        char buf[64];
        GetPrivateProfileStringA("Aim", "AimCameraLag", "0.15", buf, sizeof(buf), path);
        float v = 0.15f;
        c.aimCameraLag = ParseNumber(buf, &v) ? (v >= 0.0f && v <= 1.0f ? v : (v > 1.0f ? 1.0f : 0.15f)) : 0.15f;
    }
    {
        const int ms = (int)GetPrivateProfileIntA("Advanced", "HookDelayMs", 3000, path);
        c.hookDelayMs = (unsigned)(ms < 0 ? 0 : ms > 60000 ? 60000 : ms);
    }
    c.crosshairSize = GetScale(path, "Aim", "CrosshairSize", 0.25f, 4.0f);
    c.crosshairKick = GetFloat(path, "Aim", "CrosshairKick", c.crosshairKick);
    c.knifeCrosshair = GetBool(path, "Aim", "KnifeCrosshair", c.knifeCrosshair);
    c.knifeSpeed = GetFloat(path, "Aim", "KnifeSpeed", c.knifeSpeed);
    c.breathing = GetBool(path, "Aim", "Breathing", c.breathing);
    c.padRecoil = GetBool(path, "Aim", "Recoil", c.padRecoil);
    c.damage = GetFloat(path, "Damage", "Damage", c.damage);
    c.knifeDamage = GetFloat(path, "Damage", "KnifeDamage", c.knifeDamage);
    c.fireRate = GetScale(path, "Damage", "FireRate", 0.5f, 3.0f);
    {
        char buf[64];
        GetPrivateProfileStringA("Damage", "ShotgunSpread", "1", buf, sizeof(buf), path);
        float v = 1.0f;
        c.shotgunSpread = ParseNumber(buf, &v) && v >= 0.0f && v <= 1.0f ? v : 1.0f;  // anything else: the original
    }
    c.moveWalk = GetScale(path, "Movement", "MoveWalkSpeed", 0.5f, 2.0f);
    c.moveRun = GetScale(path, "Movement", "MoveRunSpeed", 0.5f, 2.0f);
    c.infiniteHealth = GetBool(path, "Trainer", "InfiniteHealth", c.infiniteHealth);
    c.partnerInfiniteHealth = GetBool(path, "Trainer", "PartnerInfiniteHealth", c.partnerInfiniteHealth);
    c.infiniteAmmo = GetBool(path, "Trainer", "InfiniteAmmo", c.infiniteAmmo);
    c.noReload = GetBool(path, "Trainer", "NoReload", c.noReload);
    c.infiniteGrenades = GetBool(path, "Trainer", "InfiniteGrenades", c.infiniteGrenades);
    c.noColorFilter = GetBool(path, "Visual", "NoColorFilter", c.noColorFilter);
    {
        char buf[64];
        GetPrivateProfileStringA("Performance", "FPSLimit", "144", buf, sizeof(buf), path);
        c.fpsLimit = enhancements::FpsLimit(enhancements::ParseSetting(buf, 0));
    }
    c.coopSync = GetBool(path, "Coop", "Sync", c.coopSync);
    c.keyboardMouse = GetBool(path, "Keyboard", "KeyboardMouse", c.keyboardMouse);
    c.mouseWheelWeapons = GetBool(path, "Keyboard", "MouseWheelWeapons", c.mouseWheelWeapons);
    char kbRun[32];
    GetPrivateProfileStringA("Keyboard", "Run", "Hold", kbRun, sizeof(kbRun), path);
    c.kbRunMode = !_stricmp(kbRun, "Toggle") ? RunMode::Toggle : RunMode::Hold;
    c.mouseSensitivity = GetScale(path, "Keyboard", "MouseSensitivity", 0.1f, 10.0f, 0.75f);
    c.mouseVertical = GetScale(path, "Keyboard", "MouseVertical", 0.1f, 10.0f);
    char mouseAim[32];
    GetPrivateProfileStringA("Keyboard", "MouseAim", "RE3", mouseAim, sizeof(mouseAim), path);
    c.mouseAimRE3 = _stricmp(mouseAim, "Game") != 0;
    {
        const int n = GetPrivateProfileIntA("Keyboard", "MouseSwitchCounts", c.mouseSwitchCounts, path);
        c.mouseSwitchCounts = n < 0 ? 0 : n > 100000 ? 100000 : n;
    }
    char mode[32];
    GetPrivateProfileStringA("Controls", "Mode", "Full", mode, sizeof(mode), path);
    if (!_stricmp(mode, "MeleeCamera")) {
        // Melee camera only: the original game everywhere else (the camera placement settings still apply).
        c.meleeOnly = true;
        c.meleeCamera = true;
        c.remapButtons = false;
        c.runMode = RunMode::Stock;
        c.runButton = RunButton::L3;
        c.oneButtonReload = false;
        c.quickTurn = false;
        c.stickClicks = false;
        c.knifeFromAim = false;
        c.inventoryMove = false;
        c.freeCamera = false;
        c.gunWalk = c.knifeWalk = c.reloadWalk = c.switchWalk = c.scopeWalk = false;
        c.knifeSpeed = 1.0f;
        c.crosshair = false;
        c.directHorizontal = false;
        c.aimSpeedX = c.aimSpeedY = 1.0f;
        c.aimCameraLag = 1.0f;
        c.breathing = true;
        c.padRecoil = false;
        c.damage = 0.0f;
        c.knifeDamage = 1.0f;
        c.fireRate = c.moveWalk = c.moveRun = 1.0f;
        c.shotgunSpread = 1.0f;
        c.infiniteHealth = c.partnerInfiniteHealth = false;
        c.infiniteAmmo = c.noReload = c.infiniteGrenades = false;
        c.aimDistance = c.fieldOfView = c.aimFieldOfView = 0.0f;
        Log("config: Mode=MeleeCamera (everything else as in the original game)");
    }
    g_cfg = c;
    Log("config: remap=%d run=%s/%s reload=%d quickturn=%d invmove=%d freecam=%d (%.0f/%.0f) aimwalk=%d (%.2f/%.2f) crosshair=%d (%.2f) "
        "aimspeed=%.2f/%.2f aimlag=%.2f fire=%.2f spread=%.2f move=%.2f/%.2f nofilter=%d sync=%d kbm=%d (%s, %.2f/%.2f, %s, %d) "
        "dash=%d clicks=%d knifeaim=%d coverturn=%d backstep=%d aimdist=%.0f fov=%+.0f/%+.0f grabcam=%d scopewalk=%d interact=%s "
        "trainer=hp%d partner%d ammo%d noreload%d grenade%d",
        c.remapButtons, run, c.runButton == RunButton::RB ? "RB" : "L3", c.oneButtonReload, c.quickTurn, c.inventoryMove, c.freeCamera, c.cameraSpeed, c.cameraPitchSpeed,
        c.gunWalk + c.knifeWalk * 2 + c.reloadWalk * 4 + c.switchWalk * 8, c.aimWalkAnimSpeed, c.aimWalkFastSpeed,
        c.crosshair, c.crosshairSize, c.aimSpeedX, c.aimSpeedY, c.aimCameraLag, c.fireRate, c.shotgunSpread, c.moveWalk, c.moveRun, c.noColorFilter,
        c.coopSync, c.keyboardMouse, c.kbRunMode == RunMode::Hold ? "hold" : "toggle", c.mouseSensitivity,
        c.mouseVertical, c.mouseAimRE3 ? "RE3" : "game", c.mouseSwitchCounts, c.weskerDash, c.stickClicks, c.knifeFromAim,
        c.coverTurn, c.backStep, c.aimDistance, c.fieldOfView, c.aimFieldOfView, c.grabCamera, c.scopeWalk,
        c.interactX ? "X" : "A", c.infiniteHealth, c.partnerInfiniteHealth, c.infiniteAmmo, c.noReload,
        c.infiniteGrenades);
}
