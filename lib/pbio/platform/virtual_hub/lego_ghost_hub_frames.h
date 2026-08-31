// SPDX-License-Identifier: MIT
// Copyright (c) 2026 The Pybricks Authors / lego-ghost-hub

#ifndef LEGO_GHOST_HUB_FRAMES_H
#define LEGO_GHOST_HUB_FRAMES_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Reports current execution frame data to the JavaScript bridge.
 *
 * @param line Current 1-based source line number.
 * @param function Name of the current function (or "<module>").
 * @param call_stack Array of source line numbers in the call stack from root to current.
 * @param stack_depth Number of frames in the call stack.
 */
void report_frame(int line, const char *function, int *call_stack, int stack_depth);

/**
 * Resets the frame tracking state.
 */
void lego_ghost_hub_frames_reset(void);

/**
 * Initializes and activates frame tracking in the MicroPython VM.
 */
void lego_ghost_hub_frames_init(void);

#ifdef __cplusplus
}
#endif

#endif // LEGO_GHOST_HUB_FRAMES_H
