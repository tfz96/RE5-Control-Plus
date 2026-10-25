R"RE5INI(; RE5 Control Plus 1.9.4 / RE3Controls v1.9
; English / 简体中文 - Steam RE5 Gold Edition 1.2.0, 32-bit re5dx9.exe.
; Change values only. Keep section names, keys and option names in English.
; 只修改等号右侧的值；分区名、参数名和选项值保留英文。
; 1 = on / 开启, 0 = off / 关闭. Use decimal dots / 小数使用英文句点。
; Keep comments on separate lines / 注释单独一行，不要写在数值后面。
; Most edits reload in about 1 second; HookDelayMs requires a restart.
; 多数设置约一秒热更新；HookDelayMs 修改后需重启游戏。
; Instructions and credits / 中英说明与署名：README.md

[Controls]
; Master switch, including FPS and colour-filter changes.
; 总开关，包括本版限帧修改和去黄滤镜。
Enabled=1
; Full = all controls; MeleeCamera = free camera during melee only.
; MeleeCamera skips FOV and wheel cycling; FPS/filter settings remain independent.
; Full 完整功能；MeleeCamera 仅体术自由镜头，不应用 FOV 和滚轮切枪。
; 体术专用模式下 FPS 和去黄仍单独控制。
Mode=Full
; RE3 gamepad layout: A interact, X reload, Back map, L3 run, R3 camera reset.
; 0 = stock layout; also set OneButtonReload=0 to disable extra X reload.
; 手柄 RE3 布局：A 互动、X 换弹、Back 地图、L3 跑步、R3 相机回正。
; 0 恢复游戏布局；若不要 X 额外换弹，还需 OneButtonReload=0。
RemapButtons=1
; Gamepad run: Toggle, Hold, or Stock. Keyboard run is set in [Keyboard].
; 手柄跑步：Toggle 按一下切换、Hold 按住、Stock 原作方式；键盘另见 [Keyboard]。
Run=Toggle
; L3 or RB. RB moves locate-partner / aim shoulder-swap to L3.
; 手柄跑步键 L3 或 RB；RB 会让 L3 改为定位同伴 / 瞄准换肩。
RunButton=L3
; A or X. X interacts at prompts and otherwise reloads; QTEs stay unchanged.
; 手柄互动 A 或 X；X 模式有提示时互动、无提示时换弹；QTE 不变。
InteractButton=A
; Reload without aiming: gamepad X / the game's keyboard reload key.
; 无须举枪即可换弹：手柄 X / 游戏键盘换弹键。
OneButtonReload=1
; Quick turn: gamepad B + stick toward camera; keyboard default C.
; 快速转身：手柄 B + 向相机方向拉摇杆；键盘默认 C。
QuickTurn=1
; Wesker Mercenaries dash: R3 outside aim; keyboard default Shift+X.
; 威斯克佣兵冲刺：非瞄准 R3 发动；键盘默认 Shift+X，保留原生转向。
WeskerDash=1
; Preserve R3+L3 taunt and native R3 aim actions such as night vision.
; 保留 R3+L3 挑衅及瞄准时 R3 原生动作（如夜视）；冲刺单独设置。
StickClicks=1
; Hold the knife key while aiming to draw the knife; release to return to gun aim.
; 持枪瞄准时按持刀键可直接切刀，释放后回到枪械瞄准。
KnifeFromAim=1
; Move with inventory open; gamepad d-pad / mouse selects items.
; 打开背包仍可移动；手柄十字键 / 鼠标选择物品。
InventoryMove=1

[Camera]
; Free shoulder camera and camera-relative movement; 0 = stock behaviour.
; 自由过肩相机、按镜头方向移动；0 使用原作相机和移动方式。
FreeCamera=1
; Gamepad horizontal camera speed, degrees/second; mouse has its own setting.
; 手柄自由相机水平速度，度/秒；鼠标另调 MouseSensitivity。
Speed=180
; Gamepad vertical camera speed, degrees/second.
; 手柄自由相机纵向速度，度/秒。
PitchSpeed=90
; Invert gamepad camera Y; mouse inversion is set in the game's options.
; 反转手柄相机纵向；鼠标反转在游戏选项中设置。
InvertY=0
; Camera distance behind shoulder pivot, cm; larger = farther away.
; 相机离肩部旋转支点的后方距离，厘米；越大越远。
Distance=230
; Camera pivot height above the feet, cm.
; 相机旋转支点距脚下高度，厘米。
Height=150
; Shoulder offset, cm; locate-partner swaps shoulders while aiming.
; 肩侧偏移，厘米；瞄准时可用定位同伴键换肩。
Shoulder=45
; Reset pitch, degrees; positive looks down. R3 / middle mouse resets camera.
; 回正时俯仰角，度；正数向下。R3 / 鼠标中键回正。
Tilt=8
; Extra aiming-camera distance, 0-200 cm; 0 = unchanged.
; Excludes scopes, fixed/vehicle guns and MeleeCamera-only mode.
; 瞄准镜头额外后移，0-200 厘米；0 不后移。
; 不改开镜、固定/载具机枪或体术专用模式。
AimDistance=0
; Extra normal FOV, -15 to 40 degrees; 0 = stock. Stock is usually 45 degrees.
; Preset +15 is about 60 degrees; upstream default is 0.
; 普通 FOV 增加角度，-15 至 40；0 原始。原作通常 45 度。
; 本包 +15 即约 60 度；上游默认 0。
FieldOfView=15
; Extra aiming FOV, -15 to 40 degrees; 0 = stock; preset +10, upstream 0.
; Excludes scopes, fixed/vehicle guns and MeleeCamera-only mode.
; 瞄准 FOV 增加角度，-15 至 40；0 原始；本包 +10，上游默认 0。
; 不改开镜、固定/载具机枪或体术专用模式。
AimFieldOfView=10
; Character turn speed toward movement, degrees/second; not camera sensitivity.
; 角色转向移动方向的速度，度/秒；不是相机灵敏度。
TurnSpeed=720
; Free-camera backward walk: 0 = turn toward camera, 1 = backpedal; running still turns.
; 自由相机向后走：0 转身朝相机走，1 保持朝向倒退；跑步仍转身。
BackStep=0
; Rotate camera with turning boats/platforms; 0 = keep world orientation.
; 随船、平台旋转转动相机；0 保持世界朝向。
FollowPlatform=1
; Keep free camera during melee; requires FreeCamera=1. Other full-mode features remain.
; 体术中保持自由相机，需 FreeCamera=1；与 Mode=MeleeCamera 专用模式不同。
MeleeCamera=0
; Keep free camera during ordinary grabs; lethal grabs and story cameras stay native.
; 普通抓取中保持自由镜头；致命抓取、剧情/BOSS 等镜头仍交给游戏。
GrabCamera=0
; Turn character with camera behind low cover; 0 = turn camera only.
; 低掩体中转镜头同时转角色；0 只转镜头。需要自由相机。
CoverTurn=1

[Aim]
; Move while aiming a gun / 持枪瞄准时可移动。
GunWalk=1
; Move with knife readied / 持刀时可移动。
KnifeWalk=1
; Move during one-button reload outside aim; aimed reload movement uses GunWalk.
; 非瞄准单键换弹时可移动；举枪换弹是否移动由 GunWalk 决定。
ReloadWalk=1
; Move while drawing/switching weapons / 拔枪、切枪时可移动。
SwitchWalk=1
; Walk through scopes; gamepad types B/D: locate-partner + left stick zooms.
; Mouse wheel retains zoom; 0 = stock stationary scope controls.
; 开镜时可走；手柄 B/D 布局用定位同伴键 + 左摇杆变焦。
; 键鼠仍用滚轮变焦；0 镜内站定，使用原生操作。
ScopeWalk=0
; Aimed/knife walking speed; 1.0 = stock normal walk animation speed.
; 瞄准、持刀步行速度；1.0 原作普通步行动画速度。
WalkSpeed=1.125
; Faster aimed/knife walking pace selected with the run key; not sprinting.
; 瞄准、持刀时用跑步键切换的快速步行速度；不是奔跑速度。
FastWalkSpeed=1.875
; Knife attack speed multiplier; 1.0 = stock.
; 刀攻击速度倍率；1.0 原速，1.5 为 1.5 倍。
KnifeSpeed=1.5
; Centre crosshair replaces laser; keyboard/mouse needs MouseAim=RE3.
; 中心准星替代激光；键鼠还需 MouseAim=RE3，0 恢复原生显示。
Crosshair=1
; Crosshair scale, 0.25-4; zero/negative restores 1.0 rather than hiding it.
; 准星大小 0.25-4；0 或负数恢复 1.0，不是隐藏准星。
CrosshairSize=1.0
; Horizontal aim immediately turns view/body; 0 = native arm-only aim zone.
; 水平瞄准立即带动视角、角色；0 恢复先动手臂区域，受瞄准模式影响。
DirectHorizontal=1
; Gamepad horizontal aim multiplier, 0.25-4, on top of game settings.
; 手柄水平瞄准倍率 0.25-4，叠加游戏内设置；不是鼠标速度。
AimSpeedX=1.0
; Gamepad vertical aim multiplier, 0.25-4.
; 手柄纵向瞄准倍率 0.25-4。
AimSpeedY=1.0
; Gamepad aim camera lag, 0-1; 0 = none, 1 = stock, 0.15 = mouse-like.
; 手柄瞄准相机跟随延迟 0-1；0 无拖后，1 原版，0.15 接近鼠标感。
AimCameraLag=0.15
; Crosshair opening on shots; 0 = none. Visual only, not recoil or spread.
; 开枪时准星张开幅度；0 不张开。只改显示，不等于后坐力或散布。
CrosshairKick=1.0
; Show small keyboard/mouse knife reticle; 0 = hide.
; 显示键鼠持刀小准星；0 隐藏，手柄本来没有。
KnifeCrosshair=1
; Gamepad breathing sway: 0 = steady, 1 = stock; may not add sway to mouse aim.
; 手柄呼吸晃动：0 稳定、1 原作；不保证给鼠标额外增加晃动。
Breathing=0
; Add mouse-like automatic-weapon camera recoil to gamepad aim.
; Native mouse recoil remains; this is not a mouse no-recoil switch.
; 手柄启用类似鼠标的自动武器镜头后坐力。
; 原生鼠标后坐力保留；本项不是键鼠无后坐力开关。
Recoil=1

[Damage]
; 0 = stock adaptive damage; positive = fixed outgoing multiplier instead.
; 1.0 is not stock; incoming damage is unchanged; co-op follows the host.
; 0 原作动态难度伤害；正值替代动态缩放，并非再乘一次。
; 1.0 不等于原版；玩家受伤不变；联机同步主机设置。
Damage=0
; Extra knife multiplier only when Damage>0; inactive in this profile.
; 固定 Damage 基础上额外乘给刀；仅 Damage>0 生效，本预设不强化刀伤。
KnifeDamage=2.0
; Gun fire-rate multiplier, 0.5-3; 1 = stock; excludes knives/grenades.
; 枪械射速倍率 0.5-3；1 原速，不影响刀、手雷。
FireRate=1.0
; Shotgun spread, 0-1; 1 = stock, 0.5 = half width, 0 = one line.
; 霰弹散布 0-1；1 原版、0.5 半宽、0 全部集中；无效值恢复 1。
ShotgunSpread=1.0

[Trainer]
; Optional helpers, all off by default, no hotkeys. Use solo or agreed private co-op.
; Patches memory, not save files directly; normal saves can retain inventory values.
; 可选辅助，全部默认关闭，无快捷键；用于单机或双方同意的私人联机。
; 作用于内存，不直接编辑存档；正常保存可能记录实际背包数值。
; Refill living local player HP; does not revive dying/dead characters.
; 补满存活本机角色生命；不会复活濒死、死亡角色。
InfiniteHealth=0
; Refill living local AI/split-screen partner HP; excludes remote player replicas.
; 补满存活本机 AI、分屏队友生命；不改远程网络玩家副本。
PartnerInfiniteHealth=0
; Preserve reserve ammo on reload; normal reload animations remain.
; 换弹时保留备用弹药，保留正常换弹动画；与 NoReload 可独立使用。
InfiniteAmmo=0
; Preserve magazine rounds on firing; does not cancel an ongoing reload animation.
; 开火时保留弹匣，不再需要换弹；不强行跳过已开始的换弹动作。
NoReload=0
; Preserve thrown grenade stacks; excludes proximity bombs and eggs.
; 投掷时保留手雷堆叠数量；不包括地雷、鸡蛋。
InfiniteGrenades=0

[Movement]
; Non-aimed walking multiplier, 0.5-2; 1 = stock; aimed pace is in [Aim].
; 非瞄准步行倍率 0.5-2；1 原速；瞄准步速见 [Aim]。
MoveWalkSpeed=1.0
; Non-aimed running multiplier, 0.5-2; 1 = stock.
; 非瞄准跑步倍率 0.5-2；1 原速。
MoveRunSpeed=1.0

[Visual]
; Remove level yellow tint; Special Settings filters remain. Preset 1, upstream 0.
; 去关卡黄滤镜，保留游戏特殊滤镜；本包 1，上游默认 0。
NoColorFilter=1

[Performance]
; FPS cap: 0 = original limiter calculation, or an integer from 30 to 1000.
; Select Variable frame rate in game; VSync/external limiters may still cap FPS.
; 144 is a target cap, not a monitor refresh-rate setting. Test higher FPS in play.
; 帧率上限：0 恢复原始限帧计算，或填 30-1000 整数。
; 游戏选 Variable（可变）；垂直同步、外部限帧仍可限制实际帧率。
; 144 是目标上限，不改显示器刷新率；高帧率表现仍需实际游玩验证。
FPSLimit=144

[Advanced]
; Additional startup hook delay, 0-60000 ms; 3000 = 3 seconds. Restart required.
; Try 10000 for an integrity-check failure.
; 启动条件满足后额外等待 0-60000 毫秒；3000 为 3 秒，修改后需重启。
; 完整性检查失败可尝试 10000。
HookDelayMs=3000

[Keyboard]
; Enable mod keyboard/mouse controls; 0 = stock and disables wheel gun cycling.
; 键鼠使用 mod 相机、移动操作；0 恢复原作键鼠并关闭滚轮切枪。
KeyboardMouse=1
; Keyboard run: Hold or Toggle; independent of gamepad [Controls] Run.
; 键盘跑步：Hold 按住、Toggle 按一下切换；与手柄 [Controls] Run 分开。
Run=Hold
; Mouse sensitivity multiplier, 0.1-10, on top of in-game sensitivity.
; 鼠标速度倍率 0.1-10，叠加游戏内灵敏度；越大越快。
MouseSensitivity=0.75
; Vertical mouse speed relative to horizontal, 0.1-10; invert in game options.
; 鼠标纵向相对水平速度 0.1-10；反转在游戏选项中设置。
MouseVertical=1.0
; RE3 = centred aim; Game = native aiming style selected in game options.
; RE3 强制中心瞄准；Game 使用游戏选项中的原生瞄准方式。
MouseAim=RE3
; Mouse movement counts in about 1/4 second to switch from gamepad, 0-100000.
; 0 = stock rule; actual button/wheel input switches directly.
; 手柄切回键鼠需约 1/4 秒内达到的鼠标计数，0-100000。
; 0 原作规则；按钮、滚轮实际输入可直接切换设备。
MouseSwitchCounts=30
; Down = next gun, up = previous, in inventory-slot order; skip empty/non-gun slots.
; Scopes retain zoom. Inventory, menus, lost focus and special actions discard input.
; Requires Full mode and KeyboardMouse=1; 0 = off.
; 下滚下一把、上滚上一把，按背包格子顺序跳过空位、非枪械。
; 镜内保留变焦；背包、菜单、失焦、换弹、切枪及特殊动作中丢弃输入。
; 需要 Full 模式与 KeyboardMouse=1；0 关闭。
MouseWheelWeapons=1

[Coop]
; Extra mod sync; use compatible versions on both sides. 0 does not disable online play.
; Host damage, knife damage, fire rate and spread sync; each player keeps own FOV/FPS.
; mod 额外同步，建议双方使用兼容版本；0 不会关闭游戏联机。
; 同步主机伤害、刀伤、射速、霰弹散布等；各自 FOV、FPS 独立。
Sync=1
)RE5INI"
