// SPDX-License-Identifier: MIT
// Copyright (c) 2026 The Pybricks Authors / lego-ghost-hub
//
// 5x5 hub LED matrix readout for virtualhub / wasmhub tests.

#ifndef _LEGO_GHOST_HUB_DISPLAY_H_
#define _LEGO_GHOST_HUB_DISPLAY_H_

#include <stdint.h>

#define LEGO_GHOST_HUB_DISPLAY_SIZE 5
#define LEGO_GHOST_HUB_DISPLAY_PIXELS \
    (LEGO_GHOST_HUB_DISPLAY_SIZE * LEGO_GHOST_HUB_DISPLAY_SIZE)

/** Brightness 0–100 for pixel index row * 5 + col. */
uint8_t lego_ghost_hub_display_get_pixel(uint8_t index);

/** Copy all 25 brightness values (row-major) into out[25]. */
void lego_ghost_hub_display_copy(uint8_t *out);

#endif // _LEGO_GHOST_HUB_DISPLAY_H_
