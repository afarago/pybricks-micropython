// SPDX-License-Identifier: MIT
// Copyright (c) 2026 The Pybricks Authors / hum-emu
//
// Shared HUM_EMU_PORTS map for virtualhub (PUPDevice + motor sim + sensors).

#ifndef _HUM_EMU_PORTS_H_
#define _HUM_EMU_PORTS_H_

#include <stdbool.h>
#include <stdint.h>

#include <pbio/port.h>
#include <lego/device.h>

/** Ensure the port map is loaded (from live file and/or HUM_EMU_PORTS). */
void hum_emu_ports_ensure_loaded(void);

/** Re-read HUM_EMU_PORTS_LIVE (or env) even if already loaded. */
void hum_emu_ports_reload(void);

/**
 * Configured PUP type id for a port (0 = empty).
 * Re-checks the live file so hum-emu can hot-update attachments.
 */
uint16_t hum_emu_port_type_id(pbio_port_id_t port_id);

/** True if type_id is a known encoded / DC motor PUP id. */
bool hum_emu_type_is_motor(uint16_t type_id);

/**
 * Type id to expose to the motor/counter sim for this port.
 * Non-motors and empty → LEGO_DEVICE_TYPE_ID_NONE so Motor() raises ENODEV.
 */
lego_device_type_id_t hum_emu_port_motor_type_id(pbio_port_id_t port_id);

#endif // _HUM_EMU_PORTS_H_
