# Changelog

## 1.9.4

- Publish the first regular release as RE5 Control Plus 1.9.4.
- Provide English and Chinese comments for every INI setting.
- Generate missing INI files from the same bilingual release preset.
- Rewrite the bilingual README to distinguish new features from upstream features.
- Credit tfz96's direction/testing and OpenAI Codex's AI-assisted implementation, fixes, documentation and builds.
- Reduce the installation archive to DLL, INI and one bilingual README with complete license texts.
- Preserve gameplay settings and default-off helpers from 1.9.4-beta.1.
- 发布正式版；新增逐项中英双语配置，精简三文件安装包，并保留完整署名与许可。

## 1.9.4-beta.1

- Rename the community fork from RE5Plus to RE5 Control Plus.
- Remove the inactive TurretNoOverheat option from source, generated settings and packaged configuration.
- Retain legacy DLL/INI filenames; old heat-option entries are ignored.
- Add English release instructions, third-party notices and explicit validation scope.
- Keep optional trainer settings disabled in the distributed profile.

## 1.9+RE5Plus.3

- Add configuration-only health, reserve ammo, magazine and grenade helpers.
- Validate patch signatures against live executable code.
- Correct reserve-ammo and magazine instruction patterns for the tested Steam build.

## 1.9+RE5Plus.2

- Fix mouse-wheel requests incorrectly using the give-to-partner command.
- Route wheel requests through the native equip command and discard input during inventory actions.

## 1.9+RE5Plus.1

- Add configurable FPS limiting based on RE5Fix and mouse-wheel weapon cycling.
- Package FOV, filter-removal settings and Chinese configuration documentation.
