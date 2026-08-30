// SPDX-License-Identifier: MIT
// Copyright (c) 2026 The Pybricks Authors / lego-ghost-hub

#ifndef _INTERNAL_PBDRV_LED_ARRAY_VIRTUAL_H_
#define _INTERNAL_PBDRV_LED_ARRAY_VIRTUAL_H_

#include <pbdrv/config.h>
#include <pbdrv/led.h>

#if PBDRV_CONFIG_LED_ARRAY_VIRTUAL

void pbdrv_led_array_virtual_init(pbdrv_led_array_dev_t *devs);

#else

#define pbdrv_led_array_virtual_init(devs)

#endif

#endif // _INTERNAL_PBDRV_LED_ARRAY_VIRTUAL_H_
