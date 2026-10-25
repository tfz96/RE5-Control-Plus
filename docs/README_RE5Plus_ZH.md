# RE5 Control Plus：生化危机 5 操控增强版

基于 RE3Controls `9d696ec39610a917e24a28ba3c0e829bc6878d83`（2026-10-14）。
帧率功能参考 RE5Fix `116811f573270d105c992329d870eb2e79750b8d`（2022-08-12）。
这是社区衍生版本 `1.9.4-beta.1`（原名 RE5Plus），不是上游官方更新。
DLL 与配置仍使用 `xinput1_3.dll`、`RE3Controls.ini`，兼容旧配置文件。

## plus2 修复说明

修复 plus1 滚轮切枪误触发“交给队友”的问题：改用游戏的装备请求，
不再使用赠送物品通道。已有背包装备、赠送等操作时，滚轮输入直接丢弃，避免混合操作。

从 plus1 更新：关闭游戏，只替换 `xinput1_3.dll`，保留已有 `RE3Controls.ini`，
不需要重设 FOV、144 FPS 或其他参数。之前已经交给队友的武器需在游戏内取回；
本补丁不会自动回退背包或存档。

## plus3 配置功能

新增 `[Trainer]` 分区，提供生命、弹药、免换弹和手雷功能。
所有键默认是 `0`，不使用快捷键，也不会启动外部修改器进程。

```ini
[Trainer]
InfiniteHealth=0
PartnerInfiniteHealth=0
InfiniteAmmo=0
NoReload=0
InfiniteGrenades=0
```

- `InfiniteHealth=1`：持续补满当前本机角色生命。不会复活倒地或死亡角色。
- `PartnerInfiniteHealth=1`：持续补满本机 AI 或分屏队友生命；不会改远程联机玩家副本。
- `InfiniteAmmo=1`：换弹时不扣备用弹药，保留正常换弹动画。
- `NoReload=1`：开火时不扣当前武器弹匣，因此不需要换弹。它不强行取消已开始的换弹动作。
- `InfiniteGrenades=1`：投掷时不扣手雷数量，不包含地雷或鸡蛋。

这些选项作用于本机内存，不直接编辑存档文件；正常游戏保存仍可能记录实际背包数值。它们会在游戏约一秒一次的 INI 热更新中读取；若运行时指令与 Steam 1.2.0 目标不一致，mod 会记录原因并拒绝启用。不要和 FLiNG 或 Ultimate Trainer 同时启用相同项目；为避免覆盖外部修改器，mod 不会主动还原自己未写入的代码。建议只用于单机/离线或双方明确同意的私人联机。

## 功能取舍

| 功能 | 本版处理 | 安装包预设 |
| --- | --- | --- |
| FOV 调整 | 使用 RE3Controls 的相机实现，分别设置普通/瞄准视角 | 普通 +15 度，瞄准 +10 度 |
| 超过 120 FPS | 移植 RE5Fix 的限帧计算思路，使用 MinHook 统一安装 | 144 FPS |
| 去黄滤镜 | 使用 RE3Controls 实现，保留游戏特殊滤镜 | 开启 |
| 滚轮切换武器 | 新增滚轮请求，交由游戏原生换枪流程执行 | 开启，下滚下一把、上滚上一把 |
| 阴影质量 | 按用户要求不加入 | 使用游戏设置 |

保留 RE3Controls 原有的自由相机、移动瞄准、移动换弹、手柄和键鼠操作及联机同步。
不包含 RE5Fix 的超宽屏 UI、影片、超宽屏崩溃和 4096 分辨率限制修复。
如果原先依赖这些功能，仅安装本包不会继续提供它们。

## 安装

1. 关闭游戏。备份游戏目录已有的 `xinput1_3.dll` 和 `RE3Controls.ini`（如果有）。
2. 将本包的 `xinput1_3.dll` 和 `RE3Controls.ini` 放到 `re5dx9.exe` 同一目录。
3. 游戏中将帧率选为 Variable（可变）。如果仍被限制在 60/120 FPS，检查垂直同步和显卡驱动、
   RTSS 或其他外部限帧设置。显示器刷新率在 Windows 中另行设置，本 mod 不修改刷新率。
4. 启动后查看 `RE3Controls.log`：应有 `RE5Plus FPS limiter: validated hook` 和
   `RE5Plus FPS: 144`；滚轮功能应有 `RE5Plus mouse wheel weapons: validated hook`。
   出现 `feature unavailable` 表示代码校验或挂钩未通过，对应功能没有安装。

适用目标是 Steam 生化危机 5 黄金版 1.2.0、32 位 `re5dx9.exe`，与 RE3Controls 上游一致。
旧 GFWL、DX10 和其他修改版 EXE 不在支持目标中。

本包已整合所需功能，无需同时加载旧版 RE5Fix。若以前把旧 DLL 改名成 `dinput8.dll`、
放进插件加载器目录，或通过其他加载器载入，安装前应停用那份 RE5Fix，避免重复挂钩。
`RE5Fix.ini` 不被本版读取。

## 调整

完整参数解释、默认操作与常用示例见随包 [RE3Controls 完整中文说明](RE3Controls_完整中文说明.md)。
本包 `RE3Controls.ini` 已逐项添加中文注释，配置键名和数值保持不变。

编辑同目录的 `RE3Controls.ini`：

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

FOV 数字是增加的角度，不是最终角度；设为 0 恢复原始 FOV，范围 -15 至 +40。
狙击镜、火箭筒镜头等仍保留上游的原始处理。`Mode=MeleeCamera` 模式不应用 FOV 调整。
`NoColorFilter=0` 恢复原始色调。`FPSLimit=0` 恢复游戏原始限帧计算。
缺少 `[Performance]` 的旧配置默认使用 144 FPS；无效帧率文本会退回原始限帧计算。
以上设置通常在游戏运行中约一秒后读取；菜单或暂停时可能需恢复游戏才应用。

滚轮按背包 1–9 格的顺序循环枪械，跳过空位、弹药、草药、手雷等非枪械物品。
仅作用于当前使用键鼠的本地玩家，需要 `KeyboardMouse=1` 和完整控制模式。
狙击镜等开镜状态保留游戏原有滚轮变焦。背包、菜单、失焦、换弹、切枪动画和特殊动作期间
不缓存切枪输入；快速滚动每帧最多请求一次，成功请求间至少间隔 150 毫秒。
只有一把且已装备时不重复切换。`MouseWheelWeapons=0` 关闭功能。

144 是目标上限，不保证设备始终达到 144 FPS；高于原生 120 FPS 的动画、QTE 和联机表现仍需实测。
若遇到问题，可先把 `FPSLimit` 改回 120 或 0。卸载时恢复备份 DLL 和配置。

## 验证范围

构建使用 Zig 0.13.0，输出 32 位 Windows DLL。自动测试包含配置边界、特征零匹配/多匹配、
真实 x86 SSE 指令挂钩、144/60 FPS 切换、CPU 标志保留和恢复原始计算路径。
滚轮测试实际执行独立转录的原生 x86 装备、使用、赠送和丢弃命令生成指令，
检查滚轮仅产生装备槽位与标志、未触发赠送、人物及背包数据不变。
还包含双向连续循环各 30 次（测试模拟游戏完成装备后的标志）、单把枪、格子顺序、
枪械过滤、冷却、原生背包操作优先、输入状态拦截、重复帧消费和禁用后恢复原始查询。
安装时同时校验查询函数和调用处指令。
已对本机运行时的相关游戏指令做只读核对；这些检查不替代实际游玩与联机测试，
用户已使用此前的 RE5Plus.3 完成主线通关，确认弹药功能可用。
这不代表所有关卡、武器、佣兵模式或联机环境均已验证；本次移除无效选项后的构建仍需游戏内回归测试。

局域网搜房异常曾被报告，是否与本 mod 有关尚未确认；Steam 邀请曾可加入。
公开发布时不宣称已修复这一问题。

## 源码与许可

源码、构建脚本和第三方许可随源码仓库提供。

```powershell
.\build.ps1 -Zig <zig.exe 路径> -Mode test
.\build.ps1 -Zig <zig.exe 路径> -Mode release
.\package.ps1
```

RE3Controls 与 RE5Fix 均为 MIT 许可；MinHook 为 BSD 2-Clause。
随包保留三个项目的许可文本。上游英文操作说明见 `README-RE3Controls-upstream.txt`。
