// SPDX-License-Identifier: MIT
// Copyright (c) 2026 The Pybricks Authors / lego-ghost-hub

#include <pbdrv/config.h>

#if PBDRV_CONFIG_LED_ARRAY_VIRTUAL

#include <stdint.h>
#include <string.h>

#include "led_array.h"
#include "led_array_virtual.h"
#include "lego_ghost_hub_display.h"

static uint8_t display_pixels[LEGO_GHOST_HUB_DISPLAY_PIXELS];

static pbio_error_t pbdrv_led_array_virtual_set_brightness(pbdrv_led_array_dev_t *dev, uint8_t index, uint8_t brightness) {
    (void)dev;
    if (index >= LEGO_GHOST_HUB_DISPLAY_PIXELS) {
        return PBIO_ERROR_INVALID_ARG;
    }
    display_pixels[index] = brightness > 100 ? 100 : brightness;
    return PBIO_SUCCESS;
}

static const pbdrv_led_array_funcs_t pbdrv_led_array_virtual_funcs = {
    .set_brightness = pbdrv_led_array_virtual_set_brightness,
};

void pbdrv_led_array_virtual_init(pbdrv_led_array_dev_t *devs) {
    memset(display_pixels, 0, sizeof(display_pixels));
    devs[0].funcs = &pbdrv_led_array_virtual_funcs;
}

#if defined(__EMSCRIPTEN__)
#include <emscripten.h>
#define LEGO_GHOST_HUB_KEEPALIVE EMSCRIPTEN_KEEPALIVE
#else
#define LEGO_GHOST_HUB_KEEPALIVE __attribute__((used))
#endif

LEGO_GHOST_HUB_KEEPALIVE
uint8_t lego_ghost_hub_display_get_pixel(uint8_t index) {
    if (index >= LEGO_GHOST_HUB_DISPLAY_PIXELS) {
        return 0;
    }
    return display_pixels[index];
}

LEGO_GHOST_HUB_KEEPALIVE
void lego_ghost_hub_display_copy(uint8_t *out) {
    if (out) {
        memcpy(out, display_pixels, sizeof(display_pixels));
    }
}

#endif // PBDRV_CONFIG_LED_ARRAY_VIRTUAL
