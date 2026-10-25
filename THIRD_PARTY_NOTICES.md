# Credits And Third-Party Notices

RE5 Control Plus is an independent community fork. Preserve these notices when redistributing.

## Fork Implementation And Build

- tfz96: project direction, requirements, publishing and gameplay tests of preceding RE5Plus builds.
- OpenAI Codex (AI coding assistant): assisted with fork implementation, fixes, tests, bilingual documentation, builds and release preparation under tfz96's direction.
- This attribution describes the development process and does not replace upstream authorship or license notices.

## RE3Controls

- Project: https://github.com/bananzaza/RE3Controls
- Base commit: 9d696ec39610a917e24a28ba3c0e829bc6878d83
- Copyright (c) 2026 palecvzhope
- MIT license: LICENSE
- Supplies the underlying controls, camera, FOV, filter-removal and co-op implementation.

## RE5Fix

- Project: https://github.com/Lyall/RE5Fix
- Reference commit: 116811f573270d105c992329d870eb2e79750b8d
- Copyright (c) 2022 Lyall
- MIT license: LICENSE-RE5Fix.txt
- Reference for the FPS-limiter calculation. The full RE5Fix feature set is not included.

## MinHook

- Project: https://github.com/TsudaKageyu/minhook
- Version: 1.3.4, vendored in third_party/minhook
- BSD 2-Clause license: third_party/minhook/LICENSE.txt
- Used for function hooks.

## Research References

- Puh00/Resident-Evil-5-Trainer, Copyright (c) 2020 Phu, MIT.
  Its license is retained as LICENSE-TrainerReference.txt.
- A local Cheat Engine table identifies Hose10 as its author; it was consulted when checking instruction signatures.
- User-supplied FLiNG and Raz0r/Dante trainer tools were used as feature references.

Reference trainer executables and Cheat Engine tables are not distributed with this project.
The new C++ helpers reside in src/trainer.cpp and src/trainer_core.h.
These research credits do not imply endorsement or permission to redistribute reference tools.

Resident Evil is a Capcom property. This community project is not affiliated with Capcom or Microsoft.
