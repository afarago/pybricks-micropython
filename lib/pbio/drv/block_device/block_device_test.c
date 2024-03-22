// SPDX-License-Identifier: MIT
// Copyright (c) 2024 The Pybricks Authors

// Block device dummy driver with a simple program to simplify making new ports.

#include <pbdrv/config.h>

#if PBDRV_CONFIG_BLOCK_DEVICE_TEST

#include <stdint.h>
#include <string.h>

#include <contiki.h>

#include <pbdrv/block_device.h>

/**
 * The following script is compiled using pybricksdev compile hello.py
 * in MULTI_MPY_V6.
 *
 * from pybricks.tools import wait
 *
 * print("Hello!")
 * wait(1000)
 * print("World!")
 * wait(1000)
 *
 * raise KeyboardInterrupt
 */
const uint8_t script[] = {
    0x73, 0x00, 0x00, 0x00, 0x5F, 0x5F, 0x6D, 0x61,
    0x69, 0x6E, 0x5F, 0x5F, 0x00, 0x4D, 0x06, 0x00,
    0x1F, 0x08, 0x00, 0x10, 0x68, 0x65, 0x6C, 0x6C,
    0x6F, 0x2E, 0x70, 0x79, 0x00, 0x0F, 0x08, 0x77,
    0x61, 0x69, 0x74, 0x00, 0x1C, 0x70, 0x79, 0x62,
    0x72, 0x69, 0x63, 0x6B, 0x73, 0x2E, 0x74, 0x6F,
    0x6F, 0x6C, 0x73, 0x00, 0x0C, 0x48, 0x65, 0x6C,
    0x6C, 0x6F, 0x21, 0x00, 0x0C, 0x57, 0x6F, 0x72,
    0x6C, 0x64, 0x21, 0x00, 0x81, 0x77, 0x55, 0x83,
    0x38, 0x08, 0x0C, 0x01, 0x4C, 0x27, 0x28, 0x27,
    0x48, 0x80, 0x10, 0x02, 0x2A, 0x01, 0x1B, 0x03,
    0x1C, 0x02, 0x16, 0x02, 0x59, 0x11, 0x06, 0x10,
    0x04, 0x34, 0x01, 0x59, 0x11, 0x02, 0x22, 0x87,
    0x68, 0x34, 0x01, 0x59, 0x11, 0x06, 0x10, 0x05,
    0x34, 0x01, 0x59, 0x11, 0x02, 0x22, 0x87, 0x68,
    0x34, 0x01, 0x59, 0x11, 0x07, 0x65, 0x51, 0x63,
};

#define MAX_PROGRAM_SIZE ((PBDRV_CONFIG_BLOCK_DEVICE_TEST_SIZE)-(PBDRV_CONFIG_BLOCK_DEVICE_TEST_SIZE_USER)-sizeof(uint32_t) * 2)

/**
 * Mimics data structure expected by pbsys.
 */
static struct {
    uint32_t write_size;
    uint8_t user_data[PBDRV_CONFIG_BLOCK_DEVICE_TEST_SIZE_USER];
    uint32_t program_size;
    uint8_t program_data[MAX_PROGRAM_SIZE] __attribute__((aligned(sizeof(void *))));
} blockdev = { 0 };

void pbdrv_block_device_init(void) {
    blockdev.program_size = sizeof(script);
    memcpy(blockdev.program_data, script, sizeof(script));
    blockdev.write_size = sizeof(blockdev) - sizeof(blockdev.program_data) + blockdev.program_size;
}

PT_THREAD(pbdrv_block_device_read(struct pt *pt, uint32_t offset, uint8_t *buffer, uint32_t size, pbio_error_t *err)) {

    PT_BEGIN(pt);

    // Exit on invalid size.
    if (size == 0 || offset + size > PBDRV_CONFIG_BLOCK_DEVICE_TEST_SIZE) {
        *err = PBIO_ERROR_INVALID_ARG;
        PT_EXIT(pt);
    }

    // Copy requested data to RAM.
    memcpy(buffer, (uint8_t *)&blockdev + offset, size);
    *err = PBIO_SUCCESS;

    PT_END(pt);
}

// Don't store any data in this implementation.
PT_THREAD(pbdrv_block_device_store(struct pt *pt, uint8_t *buffer, uint32_t size, pbio_error_t *err)) {
    PT_BEGIN(pt);
    *err = PBIO_SUCCESS;
    PT_END(pt);
}

#endif // PBDRV_CONFIG_BLOCK_DEVICE_FLASH_STM32
