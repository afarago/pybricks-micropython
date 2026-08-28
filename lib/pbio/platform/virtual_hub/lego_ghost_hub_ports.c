// SPDX-License-Identifier: MIT
// Copyright (c) 2026 The Pybricks Authors / lego-ghost-hub

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lego_ghost_hub_ports.h"

#define LEGO_GHOST_HUB_NUM_PORTS 6

// Empty until LEGO_GHOST_HUB_PORTS / live file is set (lego-ghost-hub always passes a map).
static uint16_t port_type_ids[LEGO_GHOST_HUB_NUM_PORTS];

#if defined(__EMSCRIPTEN__)
#include <emscripten.h>
#include <pbsys/host.h>

// Set once JS configures ports; afterwards reload() must not clobber them with
// the (empty-env) defaults, since the browser has no env var or live file.
static bool ports_set_via_js = false;

EMSCRIPTEN_KEEPALIVE
void lego_ghost_hub_ports_set(uint8_t port_index, uint16_t type_id)
{
    if (port_index < LEGO_GHOST_HUB_NUM_PORTS)
    {
        port_type_ids[port_index] = type_id;
        ports_set_via_js = true;
    }
}

// Push bytes into the host stdin ring buffer so JS can drive interactive
// programs (e.g. the bidi stdin protocol) that read via usys.stdin.
EMSCRIPTEN_KEEPALIVE
void lego_ghost_hub_stdin_write(const uint8_t *data, uint32_t size)
{
    pbsys_host_stdin_write(data, size);
}
#endif

static void lego_ghost_hub_ports_parse(const char *env)
{
    memset(port_type_ids, 0, sizeof(port_type_ids));
    if (!env || !*env)
    {
#if defined(__EMSCRIPTEN__)
        // Default layout: A,B large motor; C,D medium motor; E color; F force.
        static const uint16_t defaults[LEGO_GHOST_HUB_NUM_PORTS] = {49, 49, 48, 48, 61, 63};
        memcpy(port_type_ids, defaults, sizeof(port_type_ids));
#endif
        return;
    }

    const char *p = env;
    while (*p)
    {
        while (*p == ' ' || *p == ',')
        {
            p++;
        }
        if (!*p)
        {
            break;
        }

        char letter = *p++;
        if (letter >= 'a' && letter <= 'f')
        {
            letter = (char)(letter - 'a' + 'A');
        }
        if (letter < 'A' || letter > 'F' || *p != ':')
        {
            while (*p && *p != ',')
            {
                p++;
            }
            continue;
        }
        p++;

        int value = 0;
        int digits = 0;
        while (*p >= '0' && *p <= '9')
        {
            value = value * 10 + (*p - '0');
            p++;
            digits++;
        }
        if (digits)
        {
            port_type_ids[letter - 'A'] = (uint16_t)value;
        }
        while (*p && *p != ',')
        {
            p++;
        }
    }
}

static const char *lego_ghost_hub_ports_live_path(void)
{
    const char *path = getenv("LEGO_GHOST_HUB_PORTS_LIVE");
    if (path && *path)
    {
        return path;
    }
    return NULL;
}

static bool lego_ghost_hub_ports_read_file(const char *path, char *buf, size_t buflen)
{
    FILE *fp = fopen(path, "r");
    if (!fp)
    {
        return false;
    }
    size_t n = fread(buf, 1, buflen - 1, fp);
    fclose(fp);
    buf[n] = '\0';
    while (n > 0 && (buf[n - 1] == '\n' || buf[n - 1] == '\r' || buf[n - 1] == ' '))
    {
        buf[--n] = '\0';
    }
    return true;
}

void lego_ghost_hub_ports_reload(void)
{
    const char *live = lego_ghost_hub_ports_live_path();
    if (live)
    {
        char buf[256];
        if (lego_ghost_hub_ports_read_file(live, buf, sizeof(buf)))
        {
            lego_ghost_hub_ports_parse(buf);
            return;
        }
    }
    const char *env = getenv("LEGO_GHOST_HUB_PORTS");
#if defined(__EMSCRIPTEN__)
    // No env/live file in the browser: ports come from lego_ghost_hub_ports_set().
    // Keep those once set instead of resetting to the built-in defaults.
    if ((!env || !*env) && ports_set_via_js)
    {
        return;
    }
#endif
    lego_ghost_hub_ports_parse(env);
}

void lego_ghost_hub_ports_ensure_loaded(void)
{
    // Always refresh: lego-ghost-hub may rewrite LEGO_GHOST_HUB_PORTS_LIVE while a program runs.
    lego_ghost_hub_ports_reload();
}

uint16_t lego_ghost_hub_port_type_id(pbio_port_id_t port_id)
{
    lego_ghost_hub_ports_ensure_loaded();
    int idx = (int)port_id - (int)PBIO_PORT_ID_A;
    if (idx < 0 || idx >= LEGO_GHOST_HUB_NUM_PORTS)
    {
        return 0;
    }
    return port_type_ids[idx];
}

bool lego_ghost_hub_type_is_motor(uint16_t type_id)
{
    switch ((lego_device_type_id_t)type_id)
    {
    case LEGO_DEVICE_TYPE_ID_LPF2_MMOTOR:
    case LEGO_DEVICE_TYPE_ID_LPF2_TRAIN:
    case LEGO_DEVICE_TYPE_ID_INTERACTIVE_MOTOR:
    case LEGO_DEVICE_TYPE_ID_MOVE_HUB_MOTOR:
    case LEGO_DEVICE_TYPE_ID_TECHNIC_L_MOTOR:
    case LEGO_DEVICE_TYPE_ID_TECHNIC_XL_MOTOR:
    case LEGO_DEVICE_TYPE_ID_SPIKE_M_MOTOR:
    case LEGO_DEVICE_TYPE_ID_SPIKE_L_MOTOR:
    case LEGO_DEVICE_TYPE_ID_SPIKE_S_MOTOR:
    case LEGO_DEVICE_TYPE_ID_TECHNIC_M_ANGULAR_MOTOR:
    case LEGO_DEVICE_TYPE_ID_TECHNIC_L_ANGULAR_MOTOR:
        return true;
    default:
        // DUPLO / Move hub ids not always in the enum on every tree revision.
        return type_id == 41 || type_id == 86 || type_id == 87;
    }
}

lego_device_type_id_t lego_ghost_hub_port_motor_type_id(pbio_port_id_t port_id)
{
    uint16_t type_id = lego_ghost_hub_port_type_id(port_id);
    if (!lego_ghost_hub_type_is_motor(type_id))
    {
        return LEGO_DEVICE_TYPE_ID_NONE;
    }
    return (lego_device_type_id_t)type_id;
}
