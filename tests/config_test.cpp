#include "common.h"
#include "config.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static char directory[MAX_PATH];
static const char expected[] =
#include "default_ini.inc"
;

void Log(const char*, ...) {}
const char* ModDir() { return directory; }

static void CheckFile(const char* path, const char* data, size_t size) {
    HANDLE file = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
    assert(file != INVALID_HANDLE_VALUE);
    assert(GetFileSize(file, nullptr) == size);
    char buffer[65536];
    assert(size < sizeof(buffer));
    DWORD read = 0;
    assert(ReadFile(file, buffer, (DWORD)size, &read, nullptr) && read == size);
    CloseHandle(file);
    assert(memcmp(buffer, data, size) == 0);
}

int main() {
    char temp[MAX_PATH], path[MAX_PATH], unique[MAX_PATH];
    assert(GetTempPathA(sizeof(temp), temp));
    assert(GetTempFileNameA(temp, "r5c", 0, unique));
    assert(DeleteFileA(unique));
    assert(CreateDirectoryA(unique, nullptr));
    snprintf(directory, sizeof(directory), "%s\\", unique);
    snprintf(path, sizeof(path), "%sRE3Controls.ini", directory);

    // Exercise the real config loader and Windows profile API with the embedded UTF-8 INI.
    LoadConfig();
    CheckFile(path, expected, sizeof(expected) - 1);
    assert(Cfg().enabled && Cfg().freeCamera && Cfg().coopSync);
    assert(Cfg().fieldOfView == 15.0f && Cfg().aimFieldOfView == 10.0f);
    assert(Cfg().fpsLimit == 144 && Cfg().noColorFilter && Cfg().mouseWheelWeapons);
    assert(Cfg().runMode == RunMode::Toggle && Cfg().kbRunMode == RunMode::Hold);
    assert(!Cfg().infiniteHealth && !Cfg().partnerInfiniteHealth);
    assert(!Cfg().infiniteAmmo && !Cfg().noReload && !Cfg().infiniteGrenades);
    LoadConfig();
    CheckFile(path, expected, sizeof(expected) - 1);

    // Existing user settings must survive loading, including opt-in helpers and custom FPS/FOV.
    assert(WritePrivateProfileStringA("Performance", "FPSLimit", "120", path));
    assert(WritePrivateProfileStringA("Camera", "FieldOfView", "7", path));
    assert(WritePrivateProfileStringA("Trainer", "InfiniteAmmo", "1", path));
    LoadConfig();
    assert(Cfg().fpsLimit == 120 && Cfg().fieldOfView == 7.0f && Cfg().infiniteAmmo);
    assert(!Cfg().noReload && !Cfg().infiniteGrenades);
    LoadConfig();
    assert(Cfg().fpsLimit == 120 && Cfg().infiniteAmmo);

    WritePrivateProfileStringA(nullptr, nullptr, nullptr, path);
    assert(DeleteFileA(path));
    assert(RemoveDirectoryA(unique));
    puts("PASS: bilingual generated INI, Windows profile parsing, default-off helpers and preserved user settings.");
}
