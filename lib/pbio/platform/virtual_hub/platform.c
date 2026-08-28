// SPDX-License-Identifier: MIT
// Copyright (c) 2025 The Pybricks Authors

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#if !defined(_WIN32) && !defined(__EMSCRIPTEN__)
#include <termios.h>
#endif
#include <unistd.h>

#include "../../drv/motor_driver/motor_driver_virtual_simulation.h"
#if PBDRV_CONFIG_BLUETOOTH_BTSTACK
#include "../../drv/bluetooth/bluetooth_btstack.h"
#include "../../drv/bluetooth/bluetooth_btstack_posix.h"
#endif

#include "pbio_os_config.h"

#include <pbio/port_interface.h>
#include <pbdrv/config.h>
#include <pbdrv/ioport.h>

#include <umm_malloc.h>

const pbdrv_gpio_t pbdrv_ioport_platform_data_vcc_pin = {
    .bank = NULL,
    .pin = 0,
};

const pbdrv_ioport_platform_data_t pbdrv_ioport_platform_data[PBDRV_CONFIG_IOPORT_NUM_DEV] = {
    {
        .port_id = PBIO_PORT_ID_A,
        .motor_driver_index = 0,
        .counter_driver_index = 0,
        .external_port_index = 0,
        .i2c_driver_index = PBDRV_IOPORT_INDEX_NOT_AVAILABLE,
        .uart_driver_index = PBDRV_IOPORT_INDEX_NOT_AVAILABLE,
        .pins = NULL,
        .supported_modes = PBIO_PORT_MODE_QUADRATURE,
    },
    {
        .port_id = PBIO_PORT_ID_B,
        .motor_driver_index = 1,
        .counter_driver_index = 1,
        .external_port_index = 1,
        .i2c_driver_index = PBDRV_IOPORT_INDEX_NOT_AVAILABLE,
        .uart_driver_index = PBDRV_IOPORT_INDEX_NOT_AVAILABLE,
        .pins = NULL,
        .supported_modes = PBIO_PORT_MODE_QUADRATURE,
    },
    {
        .port_id = PBIO_PORT_ID_C,
        .motor_driver_index = 2,
        .external_port_index = 2,
        .counter_driver_index = 2,
        .i2c_driver_index = PBDRV_IOPORT_INDEX_NOT_AVAILABLE,
        .uart_driver_index = PBDRV_IOPORT_INDEX_NOT_AVAILABLE,
        .pins = NULL,
        .supported_modes = PBIO_PORT_MODE_QUADRATURE,
    },
    {
        .port_id = PBIO_PORT_ID_D,
        .motor_driver_index = 3,
        .external_port_index = 3,
        .counter_driver_index = 3,
        .i2c_driver_index = PBDRV_IOPORT_INDEX_NOT_AVAILABLE,
        .uart_driver_index = PBDRV_IOPORT_INDEX_NOT_AVAILABLE,
        .pins = NULL,
        .supported_modes = PBIO_PORT_MODE_QUADRATURE,
    },
    {
        .port_id = PBIO_PORT_ID_E,
        .motor_driver_index = 4,
        .external_port_index = 4,
        .counter_driver_index = 4,
        .i2c_driver_index = PBDRV_IOPORT_INDEX_NOT_AVAILABLE,
        .uart_driver_index = PBDRV_IOPORT_INDEX_NOT_AVAILABLE,
        .pins = NULL,
        .supported_modes = PBIO_PORT_MODE_QUADRATURE,
    },
    {
        .port_id = PBIO_PORT_ID_F,
        .motor_driver_index = 5,
        .external_port_index = 5,
        .counter_driver_index = 5,
        .i2c_driver_index = PBDRV_IOPORT_INDEX_NOT_AVAILABLE,
        .uart_driver_index = PBDRV_IOPORT_INDEX_NOT_AVAILABLE,
        .pins = NULL,
        .supported_modes = PBIO_PORT_MODE_QUADRATURE,
    },
};

#define INFINITY (1e100)

// type_id is unused: virtualhub takes PUP ids from LEGO_GHOST_HUB_PORTS (see lego_ghost_hub_ports.c).
// Keep per-port angle/endstop seeds for when a motor is attached via that map.
const pbdrv_motor_driver_virtual_simulation_platform_data_t
    pbdrv_motor_driver_virtual_simulation_platform_data[PBDRV_CONFIG_MOTOR_DRIVER_NUM_DEV] = {
    {
        .port_id = PBIO_PORT_ID_A,
        .type_id = LEGO_DEVICE_TYPE_ID_NONE,
        .initial_angle = 0,
        .initial_speed = 0,
        .endstop_angle_negative = -INFINITY,
        .endstop_angle_positive = INFINITY,
    },
    {
        .port_id = PBIO_PORT_ID_B,
        .type_id = LEGO_DEVICE_TYPE_ID_NONE,
        .initial_angle = 0,
        .initial_speed = 0,
        .endstop_angle_negative = -INFINITY,
        .endstop_angle_positive = INFINITY,
    },
    {
        .port_id = PBIO_PORT_ID_C,
        .type_id = LEGO_DEVICE_TYPE_ID_NONE,
        .initial_angle = 0,
        .initial_speed = 0,
        .endstop_angle_negative = -INFINITY,
        .endstop_angle_positive = INFINITY,
    },
    {
        .port_id = PBIO_PORT_ID_D,
        .type_id = LEGO_DEVICE_TYPE_ID_NONE,
        .initial_angle = 0,
        .initial_speed = 0,
        .endstop_angle_negative = -INFINITY,
        .endstop_angle_positive = INFINITY,
    },
    {
        .port_id = PBIO_PORT_ID_E,
        .type_id = LEGO_DEVICE_TYPE_ID_NONE,
        .initial_angle = 0,
        .initial_speed = 0,
        .endstop_angle_negative = -INFINITY,
        .endstop_angle_positive = INFINITY,
    },
    {
        .port_id = PBIO_PORT_ID_F,
        .type_id = LEGO_DEVICE_TYPE_ID_NONE,
        .initial_angle = 0,
        .initial_speed = 0,
        .endstop_angle_negative = -INFINITY,
        .endstop_angle_positive = INFINITY,
    },
};

#if PBDRV_CONFIG_BLUETOOTH_BTSTACK
const pbdrv_bluetooth_btstack_platform_data_t pbdrv_bluetooth_btstack_platform_data = {
    .transport_instance = pbdrv_bluetooth_btstack_posix_transport_instance,
    .transport_config = pbdrv_bluetooth_btstack_posix_transport_config,
    .chipset_instance = NULL,
    .control_instance = pbdrv_bluetooth_btstack_posix_control_instance,
    .er_key = (const uint8_t *)"placeholderplaceholder",
    .ir_key = (const uint8_t *)"placeholderplaceholder",
};
#endif

void *UMM_MALLOC_CFG_HEAP_ADDR = NULL;
uint32_t UMM_MALLOC_CFG_HEAP_SIZE = 0;

// The 'embedded' main.
extern void pbsys_main(void);

int main_argc;
char **main_argv;

int main(int argc, char **argv) {
    main_argc = argc;
    main_argv = argv;

    // Separate heap for large allocations - defined in linker script.
    static uint8_t umm_heap[1024 * 1024 * 2];
    umm_init_heap(umm_heap, sizeof(umm_heap));

    #if defined(PBDRV_CONFIG_RUN_ON_CI) || defined(__EMSCRIPTEN__)
    // Skip termios (fails on pipes / breaks some CI setups or Emscripten).
    #if !defined(_WIN32) && !defined(__EMSCRIPTEN__)
    int ci_stdin_flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    if (ci_stdin_flags != -1) {
        (void)fcntl(STDIN_FILENO, F_SETFL, ci_stdin_flags | O_NONBLOCK);
    }
    #endif
    pbsys_main();
    exit(0);
    return 0;
    #endif

    #if !defined(_WIN32) && !defined(__EMSCRIPTEN__)
    // Save the original terminal settings
    struct termios term_old, term_new;
    if (tcgetattr(STDIN_FILENO, &term_old) != 0) {
        printf("DEBUG: Failed to get terminal attributes\n");
        return 0;
    }
    term_new = term_old;

    // Get one char at a time instead of newline and disable CTRL+C for exit.
    term_new.c_lflag &= ~(ICANON | ECHO | ISIG);

    // MicroPython REPL expects \r for newline.
    term_new.c_iflag |= INLCR;
    term_new.c_iflag &= ~ICRNL;

    if (tcsetattr(STDIN_FILENO, TCSANOW, &term_new) != 0) {
        printf("Failed to set terminal attributes\n");
        return 0;
    }

    // Set stdin non-blocking so we can service it in the runloop like on
    // embedded hubs.
    int stdin_flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    if (stdin_flags == -1) {
        printf("Failed to get fcntl flags\n");
        return 0;
    }
    if (fcntl(STDIN_FILENO, F_SETFL, stdin_flags | O_NONBLOCK) == -1) {
        printf("Failed to set non-blocking\n");
        return 0;
    }
    #endif

    // Simulate running embedded main.
    pbsys_main();

    #if !defined(_WIN32) && !defined(__EMSCRIPTEN__)
    // Restore stdin flags.
    if (fcntl(STDIN_FILENO, F_SETFL, stdin_flags) == -1) {
        printf("Failed to restore stdin flags\n");
    }

    // Restore terminal settings.
    if (tcsetattr(STDIN_FILENO, TCSANOW, &term_old) != 0) {
        printf("Failed to restore terminal attributes\n");
        return 0;
    }
    #endif

    return 0;
}

