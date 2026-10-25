#pragma once
#include <stdint.h>

// Optional trainer-style features. They are configuration-only and default off.
// Called after every PlayerPad update, before the pad is classified as gamepad or keyboard/mouse.
void TrainerTick();
