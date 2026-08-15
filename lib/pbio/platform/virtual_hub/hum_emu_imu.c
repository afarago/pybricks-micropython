// SPDX-License-Identifier: MIT
// Copyright (c) 2026 The Pybricks Authors / hum-emu

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hum_emu_imu.h"

static float yaw_deg;
static float pitch_deg;
static float roll_deg;
static float yaw_rate_deg_s;

// Last applied live/env text so odometry updates survive until REPL rewrites.
static char last_source[256];
static bool have_source;

static bool hum_emu_imu_read_file(const char *path, char *buf, size_t buflen) {
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

static void hum_emu_imu_parse(const char *text) {
    if (!text || !*text) {
        return;
    }
    const char *p = text;
    while (*p) {
        while (*p == ' ' || *p == ',') {
            p++;
        }
        if (!*p) {
            break;
        }

        const char *key = p;
        while (*p && *p != '=' && *p != ',' && *p != ' ') {
            p++;
        }
        size_t key_len = (size_t)(p - key);
        while (*p == ' ') {
            p++;
        }
        if (*p != '=') {
            while (*p && *p != ',') {
                p++;
            }
            continue;
        }
        p++; // skip '='
        while (*p == ' ') {
            p++;
        }

        char *end = NULL;
        float value = strtof(p, &end);
        if (end == p) {
            while (*p && *p != ',') {
                p++;
            }
            continue;
        }

        if (key_len == 3 && strncasecmp(key, "yaw", 3) == 0) {
            yaw_deg = value;
        } else if (key_len == 5 && strncasecmp(key, "pitch", 5) == 0) {
            pitch_deg = value;
        } else if (key_len == 4 && strncasecmp(key, "roll", 4) == 0) {
            roll_deg = value;
        }
        p = end;
        while (*p && *p != ',') {
            p++;
        }
    }
}

void hum_emu_imu_reload(void) {
    char buf[256];
    bool got = false;

    const char *live = getenv("HUM_EMU_IMU_LIVE");
    if (live && *live) {
        got = hum_emu_imu_read_file(live, buf, sizeof(buf));
    }
    if (!got) {
        const char *env = getenv("HUM_EMU_IMU");
        if (!env) {
            env = "";
        }
        strncpy(buf, env, sizeof(buf) - 1);
        buf[sizeof(buf) - 1] = '\0';
        got = true;
    }

    if (have_source && strcmp(buf, last_source) == 0) {
        return;
    }

    strncpy(last_source, buf, sizeof(last_source) - 1);
    last_source[sizeof(last_source) - 1] = '\0';
    have_source = true;
    yaw_rate_deg_s = 0.0f;
    hum_emu_imu_parse(buf);
}

float hum_emu_imu_yaw(void) {
    hum_emu_imu_reload();
    return yaw_deg;
}

float hum_emu_imu_pitch(void) {
    hum_emu_imu_reload();
    return pitch_deg;
}

float hum_emu_imu_roll(void) {
    hum_emu_imu_reload();
    return roll_deg;
}

float hum_emu_imu_yaw_rate(void) {
    hum_emu_imu_reload();
    return yaw_rate_deg_s;
}

void hum_emu_imu_set_odometry(float yaw, float rate_deg_s) {
    // Ensure initial env/live values are loaded once before odometry takes over.
    if (!have_source) {
        hum_emu_imu_reload();
    }
    yaw_deg = yaw;
    yaw_rate_deg_s = rate_deg_s;
}

void hum_emu_imu_set_yaw(float yaw) {
    if (!have_source) {
        hum_emu_imu_reload();
    }
    yaw_deg = yaw;
    yaw_rate_deg_s = 0.0f;
}
