// SPDX-License-Identifier: MIT
// Copyright (c) 2018-2020 The Pybricks Authors

/**
 * \addtogroup LightDriver Light I/O driver
 * @{
 */

#ifndef _PBDRV_LIGHT_H_
#define _PBDRV_LIGHT_H_

#include <stdint.h>

#include <pbdrv/config.h>

#include <pbio/color.h>
#include <pbio/config.h>
#include <pbio/error.h>
#include <pbio/light.h>
#include <pbio/port.h>

/**
 * Raw RGB values for controlling RGB LEDs.
 *
 * This values should already be adjusted for variations in LED relative
 * brightness and spectral distribution.
 *
 * Value range is driver-dependant.
 */
typedef struct {
    /** The red component. */
    uint16_t r;
    /** The green component. */
    uint16_t g;
    /** The blue component. */
    uint16_t b;
} pbdrv_light_raw_rgb_t;

#if PBDRV_CONFIG_LIGHT

/**
 * Sets the color of the light. The RGB values are "raw" values, meaning that
 * setting all values to 255 may not result in a white color because of
 * different maximum intensities of the component LEDs. Use
 * ::pbdrv_light_get_rgb_for_color() to get device-specific RGB values for
 * predefined colors.
 *
 * The light may not be capabile of displaying all colors or may not have
 * adjustable brightness. If a light is only white, the color values
 * will be averaged to give the final intensity. If the light only has one or
 * two of the possible three colors, the other color values will be ignored.
 * If the light is not capabile of adjusting the intensity, values < 128 will
 * be considered "off" and >= 128 will be considered as "on".
 * @param [in]  port    The light port
 * @param [in]  rgb     The raw color component values
 * @return              ::PBIO_SUCCESS if the call was successful,
 *                      ::PBIO_ERROR_INVALID_PORT if port is not a valid port
 *                      ::PBIO_ERROR_NO_DEV if port is valid but light is not connected
 *                      ::PBIO_ERROR_IO if there was an I/O error
 */
pbio_error_t pbdrv_light_set_rgb(pbio_port_t port, const pbdrv_light_raw_rgb_t *raw);

/**
 * Gets the "raw" RGB values for a predefined color. These returned values
 * should be passed to ::pbdrv_light_set_rgb() to produce that color.
 * @param [in]  port        The light port
 * @param [in]  color       The color to look up
 * @param [out] rgb         The raw color component values
 * @return                  ::PBIO_SUCCESS if the call was successful,
 *                          ::PBIO_ERROR_INVALID_PORT if port is not a valid port
 *                          ::PBIO_ERROR_INVALID_ARG if the color value is not valid
 *                          ::PBIO_ERROR_NO_DEV if port is valid but light is not connected
 */
pbio_error_t pbdrv_light_get_rgb_for_color(pbio_port_t port, pbio_color_t color,
    pbdrv_light_raw_rgb_t *raw);

#else

static inline pbio_error_t pbdrv_light_set_rgb(pbio_port_t port, const pbdrv_light_raw_rgb_t *raw) {
    return PBIO_ERROR_NOT_SUPPORTED;
}
static inline pbio_error_t
pbdrv_light_get_rgb_for_color(pbio_port_t port, pbio_color_t color, pbdrv_light_raw_rgb_t *raw) {
    return PBIO_ERROR_NOT_SUPPORTED;
}

#endif

#endif // _PBDRV_LIGHT_H_

/** @}*/
