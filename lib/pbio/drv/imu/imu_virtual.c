// SPDX-License-Identifier: MIT
// Copyright (c) 2026 The Pybricks Authors / hum-emu
//
// Minimal virtual IMU: no async process (desktop boot has no IRQs to drive
// busy-wait). Orientation is read from HUM_EMU_IMU via pbio_imu_* overrides.

#include <pbdrv/config.h>

#if PBDRV_CONFIG_IMU_VIRTUAL

#include <stdint.h>

#include <pbdrv/imu.h>

#include "hum_emu_imu.h"

#define VIRTUAL_IMU_SAMPLE_TIME (0.01f)
#define VIRTUAL_IMU_GYRO_SCALE (0.1f)
#define VIRTUAL_IMU_ACCEL_SCALE (1.0f)

struct _pbdrv_imu_dev_t {
    pbdrv_imu_config_t config;
    pbdrv_imu_handle_frame_data_func_t handle_frame_data;
    pbdrv_imu_handle_stationary_data_func_t handle_stationary_data;
    bool initialized;
};

static pbdrv_imu_dev_t global_imu_dev;

void pbdrv_imu_init(void) {
    (void)hum_emu_imu_yaw(); // warm the live-file cache
    global_imu_dev.config.sample_time = VIRTUAL_IMU_SAMPLE_TIME;
    global_imu_dev.config.gyro_scale = VIRTUAL_IMU_GYRO_SCALE;
    global_imu_dev.config.accel_scale = VIRTUAL_IMU_ACCEL_SCALE;
    global_imu_dev.config.gyro_stationary_threshold = 20;
    global_imu_dev.config.accel_stationary_threshold = 500;
    global_imu_dev.initialized = true;
}

void pbdrv_imu_deinit(void) {
}

pbio_error_t pbdrv_imu_get_imu(pbdrv_imu_dev_t **imu_dev, pbdrv_imu_config_t **config) {
    if (!global_imu_dev.initialized) {
        return PBIO_ERROR_AGAIN;
    }
    *imu_dev = &global_imu_dev;
    *config = &global_imu_dev.config;
    return PBIO_SUCCESS;
}

bool pbdrv_imu_is_stationary(pbdrv_imu_dev_t *imu_dev) {
    (void)imu_dev;
    return true;
}

void pbdrv_imu_set_data_handlers(
    pbdrv_imu_dev_t *imu_dev,
    pbdrv_imu_handle_frame_data_func_t frame_data_func,
    pbdrv_imu_handle_stationary_data_func_t stationary_data_func) {
    imu_dev->handle_frame_data = frame_data_func;
    imu_dev->handle_stationary_data = stationary_data_func;
}

#endif // PBDRV_CONFIG_IMU_VIRTUAL
