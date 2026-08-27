// SPDX-License-Identifier: MIT
// Copyright (c) 2026 The Pybricks Authors / hum-emu

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hum_emu_ports.h"

#define HUM_EMU_NUM_PORTS 6

// Empty until HUM_EMU_PORTS / live file is set (hum-emu always passes a map).
static uint16_t port_type_ids[HUM_EMU_NUM_PORTS];

#if defined(__EMSCRIPTEN__)
#include <emscripten.h>

EMSCRIPTEN_KEEPALIVE
void hum_emu_ports_set(uint8_t port_index, uint16_t type_id) {
    if (port_index < HUM_EMU_NUM_PORTS) {
        port_type_ids[port_index] = type_id;
    }
}
#endif

static void hum_emu_ports_parse(const char *env) {
    memset(port_type_ids, 0, sizeof(port_type_ids));
    if (!env || !*env) {
        #if defined(__EMSCRIPTEN__)
        for (int i = 0; i < HUM_EMU_NUM_PORTS; i++) {
            port_type_ids[i] = 48; // LEGO_DEVICE_TYPE_ID_SPIKE_M_MOTOR
        }
        #endif
        return;
    }

    const char *p = env;
    while (*p) {
        while (*p == ' ' || *p == ',') {
            p++;
        }
        if (!*p) {
            break;
        }

        char letter = *p++;
        if (letter >= 'a' && letter <= 'f') {
            letter = (char)(letter - 'a' + 'A');
        }
        if (letter < 'A' || letter > 'F' || *p != ':') {
            while (*p && *p != ',') {
                p++;
            }
            continue;
        }
        p++;

        int value = 0;
        int digits = 0;
        while (*p >= '0' && *p <= '9') {
            value = value * 10 + (*p - '0');
            p++;
            digits++;
        }
        if (digits) {
            port_type_ids[letter - 'A'] = (uint16_t)value;
        }
        while (*p && *p != ',') {
            p++;
        }
    }
}

static const char *hum_emu_ports_live_path(void) {
    const char *path = getenv("HUM_EMU_PORTS_LIVE");
    if (path && *path) {
        return path;
    }
    return NULL;
}

static bool hum_emu_ports_read_file(const char *path, char *buf, size_t buflen) {
    FILE *fp = fopen(path, "r");
    if (!fp) {
        return false;
    }
    size_t n = fread(buf, 1, buflen - 1, fp);
    fclose(fp);
    buf[n] = '\0';
    while (n > 0 && (buf[n - 1] == '\n' || buf[n - 1] == '\r' || buf[n - 1] == ' ')) {
        buf[--n] = '\0';
    }
    return true;
}

void hum_emu_ports_reload(void) {
    const char *live = hum_emu_ports_live_path();
    if (live) {
        char buf[256];
        if (hum_emu_ports_read_file(live, buf, sizeof(buf))) {
            hum_emu_ports_parse(buf);
            return;
        }
    }
    hum_emu_ports_parse(getenv("HUM_EMU_PORTS"));
}

void hum_emu_ports_ensure_loaded(void) {
    // Always refresh: hum-emu may rewrite HUM_EMU_PORTS_LIVE while a program runs.
    hum_emu_ports_reload();
}

uint16_t hum_emu_port_type_id(pbio_port_id_t port_id) {
    hum_emu_ports_ensure_loaded();
    int idx = (int)port_id - (int)PBIO_PORT_ID_A;
    if (idx < 0 || idx >= HUM_EMU_NUM_PORTS) {
        return 0;
    }
    return port_type_ids[idx];
}

bool hum_emu_type_is_motor(uint16_t type_id) {
    switch ((lego_device_type_id_t)type_id) {
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

lego_device_type_id_t hum_emu_port_motor_type_id(pbio_port_id_t port_id) {
    uint16_t type_id = hum_emu_port_type_id(port_id);
    if (!hum_emu_type_is_motor(type_id)) {
        return LEGO_DEVICE_TYPE_ID_NONE;
    }
    return (lego_device_type_id_t)type_id;
}
