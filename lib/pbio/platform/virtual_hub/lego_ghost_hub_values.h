// SPDX-License-Identifier: MIT
// Copyright (c) 2026 The Pybricks Authors / lego-ghost-hub
//
// Live sensor readings for virtualhub (LEGO_GHOST_HUB_VALUES_LIVE).

#ifndef _LEGO_GHOST_HUB_VALUES_H_
#define _LEGO_GHOST_HUB_VALUES_H_

#include <stdbool.h>
#include <stdint.h>

#include <pbio/control.h>
#include <pbio/port.h>

/** Re-read LEGO_GHOST_HUB_VALUES_LIVE (or env). */
void lego_ghost_hub_values_reload(void);

/**
 * Get a float attr for a port (e.g. "force", "reflection").
 * Returns default_value if unset or if the attr is a string.
 */
float lego_ghost_hub_value_get(pbio_port_id_t port_id, const char *attr, float default_value);

/**
 * Get a string attr (e.g. "color"=WHITE). Returns NULL if unset / not a string.
 * Pointer valid until the next reload.
 */
const char *lego_ghost_hub_value_get_str(pbio_port_id_t port_id, const char *attr);

/** True if attr is present in the live map for this port. */
bool lego_ghost_hub_value_has(pbio_port_id_t port_id, const char *attr);

/**
 * Fill RGB_I stub channels (0..1024 each) from a Color name (WHITE, RED, …).
 * Returns false if the name is unknown.
 */
bool lego_ghost_hub_color_rgb(const char *name, int16_t rgb[3]);

/**
 * Fill SHSV stub (h, s*10, v*10) from a Color name.
 * Returns false if the name is unknown.
 */
bool lego_ghost_hub_color_shsv(const char *name, int16_t shsv[3]);

/**
 * Bind a pbio control instance to a port so Control.done/stalled can
 * resolve live overrides (Motor.done uses Motor.port_id directly).
 */
void lego_ghost_hub_bind_control(const pbio_control_t *control, pbio_port_id_t port_id);

/** Look up port for a previously bound control. Returns false if unknown. */
bool lego_ghost_hub_control_port(const pbio_control_t *control, pbio_port_id_t *port_id_out);

#endif // _LEGO_GHOST_HUB_VALUES_H_
