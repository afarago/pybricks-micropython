// SPDX-License-Identifier: MIT
// Copyright (c) 2026 The Pybricks Authors / lego-ghost-hub
//
// Live IMU orientation from lego-ghost-hub (LEGO_GHOST_HUB_IMU / LEGO_GHOST_HUB_IMU_LIVE).

#ifndef _LEGO_GHOST_HUB_IMU_H_
#define _LEGO_GHOST_HUB_IMU_H_

/** Re-read LEGO_GHOST_HUB_IMU_LIVE (or env) when the source text changes. */
void lego_ghost_hub_imu_reload(void);

/**
 * Degrees. Reloads from the live file only when its contents change, so
 * DriveBase odometry updates are not wiped every read.
 */
float lego_ghost_hub_imu_yaw(void);
float lego_ghost_hub_imu_pitch(void);
float lego_ghost_hub_imu_roll(void);

/** Yaw rate in deg/s (clockwise positive), from DriveBase odometry. */
float lego_ghost_hub_imu_yaw_rate(void);

/**
 * Push wheel-odometry heading into the IMU cache (does not write the live file).
 * @param yaw_deg   Absolute heading in degrees (clockwise positive).
 * @param rate_deg_s Heading rate in deg/s (clockwise positive).
 */
void lego_ghost_hub_imu_set_odometry(float yaw_deg, float rate_deg_s);

/** Set yaw from REPL-equivalent absolute degrees; clears yaw rate. */
void lego_ghost_hub_imu_set_yaw(float yaw_deg);

#endif // _LEGO_GHOST_HUB_IMU_H_
