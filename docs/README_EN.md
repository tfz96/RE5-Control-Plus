# RE5 Control Plus

Version 1.9.4. Community fork of RE3Controls v1.9, formerly RE5Plus.
Target: Resident Evil 5 Gold Edition, Steam 1.2.0, 32-bit re5dx9.exe.
Not an official RE3Controls or Capcom release.

## Features

- Retains upstream free camera, movement while aiming and reloading, keyboard/mouse and controller support.
- Uses upstream FOV controls and yellow colour-filter removal.
- Adds configurable FPS limiting based on RE5Fix's limiter calculation, packaged at 144 FPS.
- Adds mouse-wheel gun selection in inventory-slot order, preserving scope zoom.
- Adds optional player/local-partner health refill, reserve ammo preservation, magazine preservation and infinite grenades.
- Includes an English/Chinese-commented INI and a bilingual README.

All optional trainer settings are disabled in the release profile. They have no hotkeys.
The unsupported turret-heat option has been removed.

## Install And Upgrade

1. Exit the game and back up any existing xinput1_3.dll and RE3Controls.ini.
2. Place xinput1_3.dll and RE3Controls.ini next to re5dx9.exe.
3. Select Variable frame rate in game. VSync and external limiters may still cap FPS.
4. For an upgrade, replace the DLL and keep your existing INI. Old TurretNoOverheat entries are ignored.
5. Restore your backed-up files to uninstall. The mod does not include a save editor.

The package keeps the upstream DLL/INI names for compatibility.
Do not load another RE3Controls DLL or RE5Fix alongside this fork.
Not all RE5Fix features were ported: ultrawide UI, cutscene, resolution-limit and shadow changes are not included.

## Configuration

Edit RE3Controls.ini next to the DLL. Keys and section names remain English.
Most changes reload during play in about one second; HookDelayMs requires a restart.

```ini
[Camera]
FieldOfView=15
AimFieldOfView=10

[Visual]
NoColorFilter=1

[Performance]
FPSLimit=144

[Keyboard]
MouseWheelWeapons=1

[Trainer]
InfiniteHealth=0
PartnerInfiniteHealth=0
InfiniteAmmo=0
NoReload=0
InfiniteGrenades=0
```

FOV values are additional degrees, not final angles; 0 restores the original.
FPSLimit=0 restores the game's limiter calculation. This setting does not change the monitor refresh rate.
MouseWheelWeapons=0 restores ordinary scrolling; menus, inventory and special actions discard weapon-cycle inputs.

InfiniteHealth refills a living local character; it does not revive dead characters.
PartnerInfiniteHealth covers locally controlled characters/AI rather than a remote player's replica.
InfiniteAmmo preserves reserve ammo during reloads; NoReload preserves magazine rounds during firing.
InfiniteGrenades preserves thrown grenade stacks, not eggs or proximity bombs.
These options patch in-memory behaviour; normal game saving can retain the resulting inventory state.
Use them for solo play or private co-op with everyone's agreement.

## Validation And Reports

The owner completed the main campaign using the preceding RE5Plus.3 build and confirmed the ammo feature worked.
Version 1.9.4 is the regular release with bilingual settings and a three-file installation package.
It has not yet received a fresh in-game regression pass.
This is not certification of every chapter, weapon, game mode, frame rate or network setup.
A LAN room-discovery issue was reported; causation is unconfirmed, and Steam invitations worked in that report.

For a report, include game version, mod version, relevant INI settings, reproduction steps and RE3Controls.log.
Do not attach your Steam credentials or private account data.

## Build

Use Zig 0.13.0 on Windows:

```powershell
.\build.ps1 -Zig <path-to-zig.exe> -Mode test
.\build.ps1 -Zig <path-to-zig.exe> -Mode release
.\package.ps1
```

Tests cover the limiter, wheel command generation and trainer patch-state validation.
The release build excludes developer-only servers.
See THIRD_PARTY_NOTICES.md and the included license files for upstream credits.

The installation archive contains only xinput1_3.dll, RE3Controls.ini and README.md.
Full notices and license texts are consolidated into the packaged README; detailed guides remain in the source repository.
tfz96 directs and tests the project. OpenAI Codex (AI coding assistant) assisted with code implementation,
fixes, tests, bilingual documentation, builds and release preparation.
