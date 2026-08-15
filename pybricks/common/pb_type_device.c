// SPDX-License-Identifier: MIT
// Copyright (c) 2018-2023 The Pybricks Authors

#include "py/mpconfig.h"

#if PYBRICKS_PY_DEVICES


#include <pbio/port_interface.h>
#include <pbio/port_lump.h>

#include <lego/device.h>

#include <pybricks/common.h>
#include <pybricks/pupdevices.h>
#include <pybricks/common/pb_type_device.h>

#include <pybricks/util_pb/pb_error.h>

#include <py/runtime.h>
#include <py/mphal.h>

#if PYBRICKS_HUB_VIRTUALHUB
#include "hum_emu_ports.h"
#include "hum_emu_values.h"

static pbio_error_t pb_pup_device_stub_ready(pbio_os_state_t *state, mp_obj_t self_in) {
    (void)state;
    (void)self_in;
    return PBIO_SUCCESS;
}
#endif

/**
 * Non-blocking version of powered up data getter. Will raise exception if
 * sensor is not already in the right mode.
 *
 * Object @p self_in must be of pb_type_device_obj_base_t type or equivalent.
 *
 * @param [in]  sensor      The powered up device.
 * @param [in]  mode        Desired mode.
 * @return                  Void pointer to data.
 */
void *pb_type_device_get_data(mp_obj_t self_in, uint8_t mode) {
    pb_type_device_obj_base_t *sensor = MP_OBJ_TO_PTR(self_in);
    #if PYBRICKS_HUB_VIRTUALHUB
    if (!sensor->lump_dev) {
        // Stub LUMP payload. ForceSensor.__init__ requires CALIB[2] < CALIB[6].
        static int16_t stub_data[16];
        for (int i = 0; i < 16; i++) {
            stub_data[i] = 0;
        }
        const int16_t released = 100;
        const int16_t end = 4000;
        stub_data[1] = 0;         // ForceSensor raw_offset
        stub_data[2] = released;  // ForceSensor raw_released
        stub_data[6] = end;       // ForceSensor raw_end
        stub_data[0] = released;  // FRAW default = untouched

        pbio_port_id_t port_id = sensor->port_id;
        if (hum_emu_value_has(port_id, "force")) {
            // force_N ≈ 10 * (raw - released) / (end - released)
            float force_n = hum_emu_value_get(port_id, "force", 0.0f);
            float raw_f = (float)released + force_n * (float)(end - released) / 10.0f;
            if (raw_f < 0) {
                raw_f = 0;
            }
            if (raw_f > 30000) {
                raw_f = 30000;
            }
            stub_data[0] = (int16_t)raw_f;
        } else if (hum_emu_value_has(port_id, "touched")) {
            stub_data[0] = hum_emu_value_get(port_id, "touched", 0.0f) >= 0.5f
                ? (int16_t)(released + 20)
                : released;
        } else if (hum_emu_value_has(port_id, "distance")) {
            // ForceSensor.distance uses same FRAW scaling (~6.67mm span).
            float dist_mm = hum_emu_value_get(port_id, "distance", 0.0f);
            float raw_f = (float)released + dist_mm * (float)(end - released) / 6.67f;
            if (raw_f < 0) {
                raw_f = 0;
            }
            if (raw_f > 30000) {
                raw_f = 30000;
            }
            stub_data[0] = (int16_t)raw_f;
        }

        if (hum_emu_value_has(port_id, "reflection")) {
            // ColorSensor.reflection = (r+g+b)*100/3072 with each channel 0..1024
            float pct = hum_emu_value_get(port_id, "reflection", 0.0f);
            if (pct < 0) {
                pct = 0;
            }
            if (pct > 100) {
                pct = 100;
            }
            int16_t ch = (int16_t)(pct * 3072.0f / 300.0f); // avg of 3 channels
            if (ch > 1024) {
                ch = 1024;
            }
            stub_data[0] = ch;
            stub_data[1] = ch;
            stub_data[2] = ch;
        }
        if (hum_emu_value_has(port_id, "ambient")) {
            // ambient uses SHSV[2] / 100 → 0..100
            float pct = hum_emu_value_get(port_id, "ambient", 0.0f);
            stub_data[2] = (int16_t)(pct * 100.0f);
        }
        const char *color_name = hum_emu_value_get_str(port_id, "color");
        if (color_name) {
            if (mode == LEGO_DEVICE_MODE_PUP_COLOR_SENSOR__SHSV
                || mode == LEGO_DEVICE_MODE_PUP_COLOR_SENSOR__HSV
                || mode == LEGO_DEVICE_MODE_PUP_COLOR_SENSOR__AMBI) {
                int16_t shsv[3];
                if (hum_emu_color_shsv(color_name, shsv)) {
                    stub_data[0] = shsv[0];
                    stub_data[1] = shsv[1];
                    stub_data[2] = shsv[2];
                }
            } else {
                // RGB_I / REFLT / COLOR and other reflected-light modes
                int16_t rgb[3];
                if (hum_emu_color_rgb(color_name, rgb)) {
                    stub_data[0] = rgb[0];
                    stub_data[1] = rgb[1];
                    stub_data[2] = rgb[2];
                }
            }
        }

        return stub_data;
    }
    #endif
    void *data = NULL;
    pb_assert(pbio_port_lump_get_data(sensor->lump_dev, mode, &data));
    return data;
}

/**
 * Always-blocking version of powered up data getter. Can be used during sensor
 * or motor initialization.
 *
 * Object @p self_in must be of pb_type_device_obj_base_t type or equivalent.
 *
 * @param [in]  sensor      The powered up device.
 * @param [in]  mode        Desired mode.
 * @return                  Void pointer to data.
 */
void *pb_type_device_get_data_blocking(mp_obj_t self_in, uint8_t mode) {
    pb_type_device_obj_base_t *sensor = MP_OBJ_TO_PTR(self_in);
    #if PYBRICKS_HUB_VIRTUALHUB
    if (!sensor->lump_dev) {
        return pb_type_device_get_data(self_in, mode);
    }
    #endif
    pb_assert(pbio_port_lump_set_mode(sensor->lump_dev, mode));
    pbio_error_t err;
    while ((err = pbio_port_lump_is_ready(sensor->lump_dev)) == PBIO_ERROR_AGAIN) {
        mp_event_wait_indefinite();
    }
    pb_assert(err);
    void *data = NULL;
    pb_assert(pbio_port_lump_get_data(sensor->lump_dev, mode, &data));
    return data;
}

/**
 * Tests that a Powered Up device read or write operation has completed.
 * For reading, this means that the mode has been set and the first data for
 * the mode is ready. For writing, this means that the mode has been set and
 * data has been written to the device, including the neccessary delays for
 * discarding stale data or the time needed to externally process written data.
 *
 * See ::pbio_port_lump_is_ready for details.
 */
static pbio_error_t pb_pup_device_iter_once(pbio_os_state_t *state, mp_obj_t self_in) {
    pb_type_device_obj_base_t *sensor = MP_OBJ_TO_PTR(self_in);
    return pbio_port_lump_is_ready(sensor->lump_dev);
}

/**
 * Implements calling of async sensor methods. This is called when a (constant)
 * entry of pb_type_device_method type in a sensor class is called. It is
 * responsible for setting the sensor mode and returning an awaitable object.
 *
 * This is also called in a few places where a simple constant sensor method
 * is not sufficient, where additional wrapping code is used to dynamically
 * set the mode or return mapping, such as in the multi-purpose PUPDevice, or
 * the ColorSensor class variants, where different modes or mappings are needed
 * for a single method depending on a keyword argument.
 */
mp_obj_t pb_type_device_method_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    assert(mp_obj_is_type(self_in, &pb_type_device_method));
    pb_type_device_method_obj_t *method = MP_OBJ_TO_PTR(self_in);
    mp_arg_check_num(n_args, n_kw, 1, 1, false);

    mp_obj_t sensor_in = args[0];
    pb_type_device_obj_base_t *sensor = MP_OBJ_TO_PTR(sensor_in);
    #if PYBRICKS_HUB_VIRTUALHUB
    if (!sensor->lump_dev) {
        pb_type_async_t config = {
            .iter_once = pb_pup_device_stub_ready,
            .parent_obj = sensor_in,
            .return_map = method->get_values,
        };
        return pb_type_async_wait_or_await(&config, &sensor->last_awaitable, true);
    }
    #endif
    pb_assert(pbio_port_lump_set_mode(sensor->lump_dev, method->mode));

    pb_type_async_t config = {
        .iter_once = pb_pup_device_iter_once,
        .parent_obj = sensor_in,
        .return_map = method->get_values,
    };
    return pb_type_async_wait_or_await(&config, &sensor->last_awaitable, false);
}

/**
 * Function-like callable type for async sensor methods. This type is used for
 * constant pb_type_device_method_obj_t instances, which store a sensor mode to
 * set and a mapping function to create a return object when that data is ready.
 */
MP_DEFINE_CONST_OBJ_TYPE(
    pb_type_device_method, MP_QSTR_function, MP_TYPE_FLAG_BINDS_SELF | MP_TYPE_FLAG_BUILTIN_FUN,
    call, pb_type_device_method_call);

/**
 * Set data for a Powered Up device, such as the brightness of multiple external
 * lights on a sensor. Automatically sets the mode if not already set. Returns
 * an awaitable object that can be used to wait for the operation to complete.
 *
 * @param [in]  sensor      The powered up device.
 * @param [in]  mode        Desired mode.
 * @param [in]  data        Data to set.
 * @param [in]  size        Size of data.
 * @return                  Awaitable object.
 */
mp_obj_t pb_type_device_set_data(pb_type_device_obj_base_t *sensor, uint8_t mode, const void *data, uint8_t size) {
    #if PYBRICKS_HUB_VIRTUALHUB
    if (!sensor->lump_dev) {
        (void)mode;
        (void)data;
        (void)size;
        pb_type_async_t config = {
            .iter_once = pb_pup_device_stub_ready,
            .parent_obj = MP_OBJ_FROM_PTR(sensor),
        };
        return pb_type_async_wait_or_await(&config, &sensor->last_awaitable, true);
    }
    #endif
    pb_assert(pbio_port_lump_set_mode_with_data(sensor->lump_dev, mode, data, size));
    pb_type_async_t config = {
        .iter_once = pb_pup_device_iter_once,
        .parent_obj = MP_OBJ_FROM_PTR(sensor),
    };
    return pb_type_async_wait_or_await(&config, &sensor->last_awaitable, false);
}

void pb_device_set_lego_mode(pbio_port_t *port) {
    // Set the port mode to LEGO DCM if it is not already set.
    pbio_error_t err = pbio_port_set_mode(port, PBIO_PORT_MODE_LEGO_DCM);
    if (err == PBIO_ERROR_AGAIN) {
        // If coming from a different mode, give port some time to get started.
        // This happens when the user has a custom device and decides to switch
        // back to LEGO mode. This should be rare, so we can afford to wait.
        mp_hal_delay_ms(1000);
        err = pbio_port_set_mode(port, PBIO_PORT_MODE_LEGO_DCM);
    }
    pb_assert(err);
}

lego_device_type_id_t pb_type_device_init_class(pb_type_device_obj_base_t *self, mp_obj_t port_in, lego_device_type_id_t valid_id) {

    pb_module_tools_assert_blocking();

    pbio_port_id_t port_id = pb_type_enum_get_value(port_in, &pb_enum_type_Port);

    // Get the port instance.
    pbio_port_t *port;
    pb_assert(pbio_port_get_port(port_id, &port));

    #if PYBRICKS_HUB_VIRTUALHUB
    // No LUMP UART on virtualhub: attach typed sensors from HUM_EMU_PORTS.
    {
        uint16_t configured = hum_emu_port_type_id(port_id);
        if (configured == 0) {
            pb_assert(PBIO_ERROR_NO_DEV);
        }
        if (valid_id != LEGO_DEVICE_TYPE_ID_ANY_LUMP_UART && (uint16_t)valid_id != configured) {
            pb_assert(PBIO_ERROR_NO_DEV);
        }
        self->lump_dev = NULL;
        self->last_awaitable = NULL;
        self->port_id = port_id;
        return (lego_device_type_id_t)configured;
    }
    #else

    // Set the port mode to LEGO if it is not already set.
    pb_device_set_lego_mode(port);

    pbio_error_t err;
    lego_device_type_id_t actual_id = valid_id;
    while ((err = pbio_port_get_lump_device(port, &actual_id, &self->lump_dev)) == PBIO_ERROR_AGAIN) {
        mp_hal_delay_ms(50);
    }
    pb_assert(err);
    while ((err = pbio_port_lump_is_ready(self->lump_dev)) == PBIO_ERROR_AGAIN) {
        mp_hal_delay_ms(50);
    }
    pb_assert(err);
    self->last_awaitable = NULL;
    return actual_id;
    #endif
}

#endif // PYBRICKS_PY_PUPDEVICES
