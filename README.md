# RE5 Control Plus

**Version 1.9.4 | English / 简体中文**

A community fork of **RE3Controls v1.9** for **Resident Evil 5 Gold Edition, Steam 1.2.0 (32-bit `re5dx9.exe`)**.
生化危机 5 操控增强版，基于 RE3Controls v1.9 的社区衍生项目，适用 Steam 黄金版 1.2.0。

[Downloads / 下载](https://github.com/tfz96/RE5-Control-Plus/releases) ·
[Source / 源码](https://github.com/tfz96/RE5-Control-Plus) ·
[Full Chinese guide / 完整中文参数表](https://github.com/tfz96/RE5-Control-Plus/blob/main/docs/RE3Controls_%E5%AE%8C%E6%95%B4%E4%B8%AD%E6%96%87%E8%AF%B4%E6%98%8E.md)

## Added Features / 本版新增功能

1. **Configurable FPS cap, preset to 144 FPS / 可配置帧率上限，预设 144 FPS**
   Extends the original 120 FPS limit using the limiter calculation referenced from RE5Fix. Set `FPSLimit` to `30`–`1000`, or `0` for the original limiter. The game must use Variable frame rate.
   参考 RE5Fix 限帧计算突破原有 120 FPS 限制；`FPSLimit` 可设为 `30`–`1000`，`0` 恢复原始限帧。游戏需选择 Variable（可变），本项不修改显示器刷新率。
2. **Mouse-wheel weapon cycling / 滚轮切换武器**
   Scroll down for the next gun, up for the previous gun, in inventory-slot order. Empty and non-gun slots are skipped. Uses the native equip command; scopes retain wheel zoom. Inventory, menus and special actions discard cycle input.
   下滚下一把、上滚上一把，按背包格子顺序跳过空位和非枪械。使用原生装备指令；开镜保留滚轮变焦，背包、菜单及特殊动作中不缓存切枪输入。已修复早期版本误赠送武器的问题。
3. **Optional health helpers / 可选生命辅助**
   `InfiniteHealth` refills living local-player HP; `PartnerInfiniteHealth` covers local AI or split-screen partners. Neither revives dying/dead characters or changes remote-player replicas.
   `InfiniteHealth` 补满存活本机角色生命；`PartnerInfiniteHealth` 补满本机 AI 或分屏队友生命。不会复活濒死、死亡角色，也不修改远程玩家副本。
4. **Optional ammunition helpers / 可选弹药与免换弹**
   `InfiniteAmmo` preserves reserve ammo during reloads. `NoReload` preserves magazine rounds during firing. Enable either or both through the INI.
   `InfiniteAmmo` 换弹时不扣备用弹药，保留正常换弹动画；`NoReload` 开火时不扣弹匣。两项可独立启用。
5. **Optional infinite grenades / 可选无限手雷**
   `InfiniteGrenades` preserves thrown grenade stacks; eggs and proximity bombs are excluded.
   `InfiniteGrenades` 投掷时不扣手雷堆叠，不包括鸡蛋、地雷。
6. **Bilingual configuration and instructions / 中英双语配置与说明**
   Every INI setting has English and Chinese comments. Missing INI files are generated with the same bilingual release preset. Helpers default to **off**, with **no hotkeys** or external trainer required.
   每个 INI 设置均有中英文注释；配置缺失时自动生成相同的双语预设。辅助功能全部**默认关闭**，**没有快捷键**，无需另开修改器。

## Retained Features / 沿用原项目功能

- **Free shoulder camera and camera-relative movement / 自由过肩相机与相机方向移动**.
- **Movement while aiming, reloading and switching weapons / 移动瞄准、移动换弹、移动切枪**.
- **Keyboard/mouse and controller controls / 键鼠与手柄操作支持**.
- **FOV adjustment / FOV 调整**: upstream implementation, preset to normal `+15°` and aiming `+10°`; these are additions to the game's FOV. / 使用上游实现，预设普通 `+15°`、瞄准 `+10°`，数字是增加角度。
- **Yellow colour-filter removal / 去黄滤镜**: upstream implementation, enabled in this preset; Special Settings filters remain. / 使用上游实现，本包预设开启，保留游戏特殊滤镜。
- **Co-op synchronization / 联机同步**: upstream implementation; FOV and FPS remain local settings. / 沿用上游实现，各自 FOV、FPS 独立。

## Installation / 安装与升级

1. Close the game. Back up any existing `xinput1_3.dll` and `RE3Controls.ini`.
   关闭游戏，备份已有 DLL 和 INI。
2. Extract `xinput1_3.dll` and `RE3Controls.ini` beside **`re5dx9.exe`**.
   将这两个文件解压到 **`re5dx9.exe` 同目录**。
3. Select **Variable** frame rate in the game's options. VSync, GPU drivers and external limiters may still cap FPS.
   游戏帧率选择 **Variable（可变）**；垂直同步、显卡驱动或外部限帧仍可能限制实际帧率。
4. For upgrades, replace the DLL and keep your customized INI. To adopt the bilingual comments, transfer your values into the new INI first.
   升级可仅替换 DLL、保留已有配置；需要新版双语注释时，先把自己的数值迁入新 INI。
5. To uninstall, remove this mod's DLL/INI and restore your backups.
   卸载时移除本 mod 的 DLL、INI，并恢复备份。

The release contains **three files**: `xinput1_3.dll`, `RE3Controls.ini`, and this `README.md` (instructions, credits and licenses).
安装包仅含上述三个文件；说明、署名和许可合并在本文档。

Do not load a second RE3Controls DLL or RE5Fix alongside this fork. Legacy DLL/INI filenames are retained for upgrades.
请勿同时加载另一份 RE3Controls 或 RE5Fix；沿用原 DLL、INI 文件名以兼容升级。

## Common Settings / 常用设置

Edit `RE3Controls.ini`; preserve English section/key names and put comments on their own lines. Most changes reload in about one second; `HookDelayMs` requires a restart.
编辑 INI 时保留英文分区、参数名，注释单独一行；多数设置约一秒热更新，`HookDelayMs` 需重启。

| Section / 分区 | Setting / 参数 | Release preset / 预设 | Effect / 作用 |
| --- | --- | --- | --- |
| Camera | FieldOfView | 15 | Extra normal FOV degrees / 普通视角增加角度 |
| Camera | AimFieldOfView | 10 | Extra aimed FOV degrees / 瞄准视角增加角度 |
| Visual | NoColorFilter | 1 | Remove level yellow tint / 去关卡黄滤镜 |
| Performance | FPSLimit | 144 | Target FPS cap; 0 = original / 目标帧率上限；0 原始 |
| Keyboard | MouseWheelWeapons | 1 | Wheel gun cycling; 0 = off / 滚轮切枪；0 关闭 |
| Trainer | InfiniteHealth | 0 | Refill living local-player HP / 补满存活本机角色生命 |
| Trainer | PartnerInfiniteHealth | 0 | Refill local partner HP / 补满本机队友生命 |
| Trainer | InfiniteAmmo | 0 | Preserve reserve ammo / 备用弹药不减 |
| Trainer | NoReload | 0 | Preserve magazine rounds / 弹匣不减 |
| Trainer | InfiniteGrenades | 0 | Preserve grenade stacks / 手雷不减 |

Set a helper to `1` to enable it. Use helpers in solo play or private co-op with everyone's agreement. They patch memory; normal saves can retain resulting inventory values.
将辅助项设为 `1` 即可开启。用于单机或双方同意的私人联机；功能作用于内存，正常保存仍可能记录实际背包数值。

Wheel cycling requires `Mode=Full` and `KeyboardMouse=1`. FOV changes do not apply to MeleeCamera-only mode; scopes and fixed/vehicle guns retain their upstream handling.
滚轮切枪需要 `Mode=Full`、`KeyboardMouse=1`；体术专用模式不应用 FOV 调整，开镜及固定、载具机枪沿用上游处理。

## Validation / 验证范围

Native tests cover FPS hooks, wheel equip/give separation and helper patch-state validation. Release checks verify bilingual INI coverage, generated defaults, the three-file package, x86 DLL metadata and packaged file contents.
原生测试覆盖 FPS 挂钩、滚轮装备与赠送指令分离、辅助补丁状态；发布检查覆盖双语配置、自动生成预设、三文件打包、x86 DLL 元数据和包内文件一致性。

The preceding RE5Plus.3 build was used to complete the main campaign, with ammunition confirmed working. Version 1.9.4 has not received a fresh in-game regression pass; this does not certify every chapter, weapon, high-FPS or network setup. A reported LAN discovery issue remains unconfirmed.
此前 RE5Plus.3 已用于主线通关，弹药功能确认可用。1.9.4 尚未重新完成游戏内回归测试，不代表所有关卡、武器、高帧率或联机环境均已验证；曾报告局域网搜房异常，原因尚未确认。

Report issues with the game/mod version, relevant INI values, reproduction steps and `RE3Controls.log`.
反馈问题请附游戏及 mod 版本、相关 INI 设置、复现步骤和日志。

## Build / 源码构建

Use Zig 0.13.0 on Windows / Windows 使用 Zig 0.13.0：

```powershell
.\build.ps1 -Zig <path-to-zig.exe> -Mode test
.\build.ps1 -Zig <path-to-zig.exe> -Mode release
.\package.ps1
.\tests\release_test.ps1 -PackageDirectory .\build\RE5-Control-Plus-1.9.4
```

`main` contains published code; `develop` is for ongoing work. The release excludes developer-only servers.
`main` 保存发布代码，`develop` 用于后续开发；正式构建不包含开发服务器。

## Credits / 作者与构建署名

- **tfz96**: project direction, requirements, release ownership and in-game testing of the preceding RE5Plus builds. / 项目发起、功能需求、发布维护及此前 RE5Plus 版本的游戏实测。
- **OpenAI Codex (AI coding assistant / AI 编程助手)**: assisted with this fork's code implementation, fixes, tests, bilingual documentation, build and release preparation under tfz96's direction. / 根据 tfz96 的需求协助完成本衍生版代码扩展、修复、测试、双语文档、构建及发布准备。
- **palecvzhope / bananzaza**: RE3Controls v1.9, the original controls/camera/FOV/filter/co-op implementation; MIT. / 原项目作者及基础功能实现。
- **Lyall**: RE5Fix FPS-limiter reference; MIT. / 帧率计算参考项目作者。
- **MinHook contributors**: function-hook library 1.3.4; BSD 2-Clause. / 函数挂钩库。
- **Phu / Puh00**: MIT-licensed trainer reference. **Hose10**: local instruction-signature reference table. FLiNG and Raz0r/Dante tools were feature references. / 修改器与指令签名研究参考。

This is an independent fork, not an official upstream or Capcom release. Research credits do not imply endorsement or rights to redistribute reference tools.
本项目为独立社区衍生版，并非原项目或 Capcom 官方发布；参考署名不代表背书或参考工具的再分发授权。

Source license texts are preserved in the repository; the downloadable three-file package appends the full notices and license texts below.
源码仓库保留各项目许可原文；三文件安装包在本文末尾合并完整署名与许可文本。
