// SPDX-License-Identifier: MIT
// Copyright (c) 2026 The Pybricks Authors / hum-emu

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hum_emu_values.h"

#define HUM_EMU_NUM_PORTS 6
#define HUM_EMU_MAX_ATTRS 12
#define HUM_EMU_ATTR_LEN 24
#define HUM_EMU_STR_LEN 24

typedef struct {
    char name[HUM_EMU_ATTR_LEN];
    float value;
    char str[HUM_EMU_STR_LEN];
    bool is_str;
    bool set;
} hum_emu_attr_t;

static hum_emu_attr_t attrs[HUM_EMU_NUM_PORTS][HUM_EMU_MAX_ATTRS];

static void hum_emu_values_clear(void) {
    memset(attrs, 0, sizeof(attrs));
}

static hum_emu_attr_t *hum_emu_attr_slot(int port_idx, const char *name, bool create) {
    hum_emu_attr_t *empty = NULL;
    for (int i = 0; i < HUM_EMU_MAX_ATTRS; i++) {
        hum_emu_attr_t *a = &attrs[port_idx][i];
        if (a->set && strcmp(a->name, name) == 0) {
            return a;
        }
        if (!a->set && empty == NULL) {
            empty = a;
        }
    }
    if (create && empty) {
        strncpy(empty->name, name, HUM_EMU_ATTR_LEN - 1);
        empty->name[HUM_EMU_ATTR_LEN - 1] = '\0';
        return empty;
    }
    return NULL;
}

static void hum_emu_normalize_color_token(char *dst, size_t dstlen, const char *src) {
    // Accept WHITE / white / Color.WHITE / Color.white / GREY→GRAY
    const char *p = src;
    if ((p[0] == 'C' || p[0] == 'c')
        && (p[1] == 'o' || p[1] == 'O')
        && (p[2] == 'l' || p[2] == 'L')
        && (p[3] == 'o' || p[3] == 'O')
        && (p[4] == 'r' || p[4] == 'R')
        && p[5] == '.') {
        p += 6;
    }
    size_t i = 0;
    while (*p && *p != ',' && *p != ' ' && i + 1 < dstlen) {
        dst[i++] = (char)toupper((unsigned char)*p++);
    }
    dst[i] = '\0';
    if (strcmp(dst, "GREY") == 0) {
        strncpy(dst, "GRAY", dstlen);
        dst[dstlen - 1] = '\0';
    }
}

static void hum_emu_values_parse(const char *text) {
    hum_emu_values_clear();
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

        char letter = *p++;
        if (letter >= 'a' && letter <= 'f') {
            letter = (char)(letter - 'a' + 'A');
        }
        if (letter < 'A' || letter > 'F' || *p != '.') {
            while (*p && *p != ',') {
                p++;
            }
            continue;
        }
        p++; // skip '.'

        char name[HUM_EMU_ATTR_LEN];
        int ni = 0;
        while (*p && *p != '=' && *p != ',' && ni < HUM_EMU_ATTR_LEN - 1) {
            name[ni++] = *p++;
        }
        name[ni] = '\0';
        if (*p != '=' || ni == 0) {
            while (*p && *p != ',') {
                p++;
            }
            continue;
        }
        p++; // skip '='

        hum_emu_attr_t *slot = hum_emu_attr_slot(letter - 'A', name, true);
        if (!slot) {
            while (*p && *p != ',') {
                p++;
            }
            continue;
        }

        char *end = NULL;
        float value = strtof(p, &end);
        bool prefer_str = (strcmp(name, "color") == 0);
        if (!prefer_str && end != p && (*end == '\0' || *end == ',' || *end == ' ')) {
            slot->value = value;
            slot->is_str = false;
            slot->str[0] = '\0';
            slot->set = true;
            p = end;
        } else {
            char token[HUM_EMU_STR_LEN];
            int ti = 0;
            while (*p && *p != ',' && ti < HUM_EMU_STR_LEN - 1) {
                if (*p != ' ') {
                    token[ti++] = *p;
                }
                p++;
            }
            token[ti] = '\0';
            if (strcmp(name, "color") == 0) {
                hum_emu_normalize_color_token(slot->str, sizeof(slot->str), token);
            } else {
                strncpy(slot->str, token, HUM_EMU_STR_LEN - 1);
                slot->str[HUM_EMU_STR_LEN - 1] = '\0';
            }
            slot->is_str = true;
            slot->value = 0;
            slot->set = true;
        }
        while (*p && *p != ',') {
            p++;
        }
    }
}

static bool hum_emu_values_read_file(const char *path, char *buf, size_t buflen) {
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

void hum_emu_values_reload(void) {
    const char *live = getenv("HUM_EMU_VALUES_LIVE");
    if (live && *live) {
        char buf[512];
        if (hum_emu_values_read_file(live, buf, sizeof(buf))) {
            hum_emu_values_parse(buf);
            return;
        }
    }
    hum_emu_values_parse(getenv("HUM_EMU_VALUES"));
}

bool hum_emu_value_has(pbio_port_id_t port_id, const char *attr) {
    hum_emu_values_reload();
    int idx = (int)port_id - (int)PBIO_PORT_ID_A;
    if (idx < 0 || idx >= HUM_EMU_NUM_PORTS || !attr) {
        return false;
    }
    hum_emu_attr_t *slot = hum_emu_attr_slot(idx, attr, false);
    return slot && slot->set;
}

float hum_emu_value_get(pbio_port_id_t port_id, const char *attr, float default_value) {
    hum_emu_values_reload();
    int idx = (int)port_id - (int)PBIO_PORT_ID_A;
    if (idx < 0 || idx >= HUM_EMU_NUM_PORTS || !attr) {
        return default_value;
    }
    hum_emu_attr_t *slot = hum_emu_attr_slot(idx, attr, false);
    if (slot && slot->set && !slot->is_str) {
        return slot->value;
    }
    return default_value;
}

const char *hum_emu_value_get_str(pbio_port_id_t port_id, const char *attr) {
    hum_emu_values_reload();
    int idx = (int)port_id - (int)PBIO_PORT_ID_A;
    if (idx < 0 || idx >= HUM_EMU_NUM_PORTS || !attr) {
        return NULL;
    }
    hum_emu_attr_t *slot = hum_emu_attr_slot(idx, attr, false);
    if (slot && slot->set && slot->is_str && slot->str[0]) {
        return slot->str;
    }
    return NULL;
}

bool hum_emu_color_rgb(const char *name, int16_t rgb[3]) {
    if (!name || !rgb) {
        return false;
    }
    char n[HUM_EMU_STR_LEN];
    hum_emu_normalize_color_token(n, sizeof(n), name);
    // Channels are 0..1024 as used by ColorSensor RGB_I
    if (strcmp(n, "WHITE") == 0) {
        rgb[0] = rgb[1] = rgb[2] = 1024;
    } else if (strcmp(n, "NONE") == 0 || strcmp(n, "BLACK") == 0) {
        rgb[0] = rgb[1] = rgb[2] = 0;
    } else if (strcmp(n, "GRAY") == 0) {
        rgb[0] = rgb[1] = rgb[2] = 512;
    } else if (strcmp(n, "RED") == 0) {
        rgb[0] = 1024;
        rgb[1] = 0;
        rgb[2] = 0;
    } else if (strcmp(n, "ORANGE") == 0 || strcmp(n, "BROWN") == 0) {
        rgb[0] = 1024;
        rgb[1] = 400;
        rgb[2] = 0;
    } else if (strcmp(n, "YELLOW") == 0) {
        rgb[0] = 1024;
        rgb[1] = 1024;
        rgb[2] = 0;
    } else if (strcmp(n, "GREEN") == 0) {
        rgb[0] = 0;
        rgb[1] = 1024;
        rgb[2] = 0;
    } else if (strcmp(n, "CYAN") == 0) {
        rgb[0] = 0;
        rgb[1] = 1024;
        rgb[2] = 1024;
    } else if (strcmp(n, "BLUE") == 0) {
        rgb[0] = 0;
        rgb[1] = 0;
        rgb[2] = 1024;
    } else if (strcmp(n, "VIOLET") == 0) {
        rgb[0] = 512;
        rgb[1] = 0;
        rgb[2] = 1024;
    } else if (strcmp(n, "MAGENTA") == 0) {
        rgb[0] = 1024;
        rgb[1] = 0;
        rgb[2] = 1024;
    } else {
        return false;
    }
    return true;
}

bool hum_emu_color_shsv(const char *name, int16_t shsv[3]) {
    if (!name || !shsv) {
        return false;
    }
    char n[HUM_EMU_STR_LEN];
    hum_emu_normalize_color_token(n, sizeof(n), name);
    // SHSV: h degrees, s and v in 0..1000 (sensor scales /10 → 0..100)
    int h = 0, s = 0, v = 0;
    if (strcmp(n, "WHITE") == 0) {
        v = 1000;
    } else if (strcmp(n, "NONE") == 0) {
        /* zeros */
    } else if (strcmp(n, "BLACK") == 0) {
        v = 100;
    } else if (strcmp(n, "GRAY") == 0) {
        v = 500;
    } else if (strcmp(n, "RED") == 0) {
        h = 0;
        s = 1000;
        v = 1000;
    } else if (strcmp(n, "ORANGE") == 0) {
        h = 30;
        s = 1000;
        v = 1000;
    } else if (strcmp(n, "BROWN") == 0) {
        h = 30;
        s = 1000;
        v = 500;
    } else if (strcmp(n, "YELLOW") == 0) {
        h = 60;
        s = 1000;
        v = 1000;
    } else if (strcmp(n, "GREEN") == 0) {
        h = 120;
        s = 1000;
        v = 1000;
    } else if (strcmp(n, "CYAN") == 0) {
        h = 180;
        s = 1000;
        v = 1000;
    } else if (strcmp(n, "BLUE") == 0) {
        h = 240;
        s = 1000;
        v = 1000;
    } else if (strcmp(n, "VIOLET") == 0) {
        h = 270;
        s = 1000;
        v = 1000;
    } else if (strcmp(n, "MAGENTA") == 0) {
        h = 300;
        s = 1000;
        v = 1000;
    } else {
        return false;
    }
    shsv[0] = (int16_t)h;
    shsv[1] = (int16_t)s;
    shsv[2] = (int16_t)v;
    return true;
}

#define HUM_EMU_MAX_CTRL 8

typedef struct {
    const pbio_control_t *control;
    pbio_port_id_t port_id;
    bool set;
} hum_emu_ctrl_bind_t;

static hum_emu_ctrl_bind_t ctrl_binds[HUM_EMU_MAX_CTRL];

void hum_emu_bind_control(const pbio_control_t *control, pbio_port_id_t port_id) {
    if (!control) {
        return;
    }
    hum_emu_ctrl_bind_t *empty = NULL;
    for (int i = 0; i < HUM_EMU_MAX_CTRL; i++) {
        hum_emu_ctrl_bind_t *b = &ctrl_binds[i];
        if (b->set && b->control == control) {
            b->port_id = port_id;
            return;
        }
        if (!b->set && empty == NULL) {
            empty = b;
        }
    }
    if (empty) {
        empty->control = control;
        empty->port_id = port_id;
        empty->set = true;
    }
}

bool hum_emu_control_port(const pbio_control_t *control, pbio_port_id_t *port_id_out) {
    if (!control || !port_id_out) {
        return false;
    }
    for (int i = 0; i < HUM_EMU_MAX_CTRL; i++) {
        hum_emu_ctrl_bind_t *b = &ctrl_binds[i];
        if (b->set && b->control == control) {
            *port_id_out = b->port_id;
            return true;
        }
    }
    return false;
}
