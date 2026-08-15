// SPDX-License-Identifier: MIT
// Copyright (c) 2026 The Pybricks Authors / hum-emu
//
// Live IMU orientation from hum-emu (HUM_EMU_IMU / HUM_EMU_IMU_LIVE).

#ifndef _HUM_EMU_IMU_H_
#define _HUM_EMU_IMU_H_

/** Re-read HUM_EMU_IMU_LIVE (or env) when the source text changes. */
void hum_emu_imu_reload(void);

/**
 * Degrees. Reloads from the live file only when its contents change, so
 * DriveBase odometry updates are not wiped every read.
 */
float hum_emu_imu_yaw(void);
float hum_emu_imu_pitch(void);
float hum_emu_imu_roll(void);

/** Yaw rate in deg/s (clockwise positive), from DriveBase odometry. */
float hum_emu_imu_yaw_rate(void);

/**
 * Push wheel-odometry heading into the IMU cache (does not write the live file).
 * @param yaw_deg   Absolute heading in degrees (clockwise positive).
 * @param rate_deg_s Heading rate in deg/s (clockwise positive).
 */
void hum_emu_imu_set_odometry(float yaw_deg, float rate_deg_s);

/** Set yaw from REPL-equivalent absolute degrees; clears yaw rate. */
void hum_emu_imu_set_yaw(float yaw_deg);

#endif // _HUM_EMU_IMU_H_
