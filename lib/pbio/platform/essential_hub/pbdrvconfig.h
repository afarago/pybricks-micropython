// SPDX-License-Identifier: MIT
// Copyright (c) 2018-2021 The Pybricks Authors

#ifndef _PBDRVCONFIG_H_
#define _PBDRVCONFIG_H_

#define PBDRV_CONFIG_ADC                            (1)
#define PBDRV_CONFIG_ADC_STM32_HAL                  (1)
#define PBDRV_CONFIG_ADC_STM32_HAL_ADC_INSTANCE     ADC1
#define PBDRV_CONFIG_ADC_STM32_HAL_ADC_NUM_CHANNELS 5
#define PBDRV_CONFIG_ADC_STM32_HAL_DMA_INSTANCE     DMA2_Stream0
#define PBDRV_CONFIG_ADC_STM32_HAL_DMA_CHANNEL      DMA_CHANNEL_0
#define PBDRV_CONFIG_ADC_STM32_HAL_DMA_IRQ          DMA2_Stream0_IRQn
#define PBDRV_CONFIG_ADC_STM32_HAL_TIMER_INSTANCE   TIM8
#define PBDRV_CONFIG_ADC_STM32_HAL_TIMER_TRIGGER    ADC_EXTERNALTRIGCONV_T8_TRGO

#define PBDRV_CONFIG_BATTERY                        (1)
#define PBDRV_CONFIG_BATTERY_ADC                    (1)
#define PBDRV_CONFIG_BATTERY_ADC_VOLTAGE_CH         1
#define PBDRV_CONFIG_BATTERY_ADC_VOLTAGE_RAW_MAX    4096
#define PBDRV_CONFIG_BATTERY_ADC_VOLTAGE_SCALED_MAX 8768
#define PBDRV_CONFIG_BATTERY_ADC_CURRENT_CORRECTION 3
#define PBDRV_CONFIG_BATTERY_ADC_CURRENT_CH         0
#define PBDRV_CONFIG_BATTERY_ADC_CURRENT_RAW_OFFSET 0
#define PBDRV_CONFIG_BATTERY_ADC_CURRENT_RAW_MAX    4096
#define PBDRV_CONFIG_BATTERY_ADC_CURRENT_SCALED_MAX 2001
#define PBDRV_CONFIG_BATTERY_ADC_TYPE               (2)
#define PBDRV_CONFIG_BATTERY_ADC_TEMPERATURE        (1)
#define PBDRV_CONFIG_BATTERY_ADC_TEMPERATURE_CH     2

#define PBDRV_CONFIG_BLUETOOTH                      (1)
#define PBDRV_CONFIG_BLUETOOTH_BTSTACK              (1)
#define PBDRV_CONFIG_BLUETOOTH_BTSTACK_STM32_UART   (1)
#undef PBDRV_CONFIG_BLUETOOTH_BTSTACK_HUB_VARIANT_ADDR

#define PBDRV_CONFIG_BUTTON                         (1)
#define PBDRV_CONFIG_BUTTON_GPIO                    (1)
#define PBDRV_CONFIG_BUTTON_GPIO_NUM_BUTTON         (1)

#define PBDRV_CONFIG_CLOCK                          (1)
#define PBDRV_CONFIG_CLOCK_STM32                    (1)

#define PBDRV_CONFIG_COUNTER                        (1)
#define PBDRV_CONFIG_COUNTER_NUM_DEV                (2)

#define PBDRV_CONFIG_GPIO                           (1)
#define PBDRV_CONFIG_GPIO_STM32F4                   (1)

#define PBDRV_CONFIG_IOPORT                         (1)
#define PBDRV_CONFIG_IOPORT_LPF2                    (1)
#define PBDRV_CONFIG_IOPORT_LPF2_NUM_PORTS          (2)
#define PBDRV_CONFIG_IOPORT_LPF2_FIRST_PORT         PBIO_PORT_ID_A
#define PBDRV_CONFIG_IOPORT_LPF2_LAST_PORT          PBIO_PORT_ID_B

#define PBDRV_CONFIG_LED                            (1)
#define PBDRV_CONFIG_LED_NUM_DEV                    (2)
#define PBDRV_CONFIG_LED_PWM                        (1)
#define PBDRV_CONFIG_LED_PWM_NUM_DEV                (2)

#define PBDRV_CONFIG_PWM                            (1)
#define PBDRV_CONFIG_PWM_NUM_DEV                    (4)
#define PBDRV_CONFIG_PWM_STM32_TIM                  (1)
#define PBDRV_CONFIG_PWM_STM32_TIM_NUM_DEV          (3)
#define PBDRV_CONFIG_PWM_STM32_TIM_EXTRA_FLAGS      (1)
#define PBDRV_CONFIG_PWM_LP50XX_STM32               (1)
#define PBDRV_CONFIG_PWM_LP50XX_STM32_NUM_DEV       (1)

#define PBDRV_CONFIG_RESET                          (1)
#define PBDRV_CONFIG_RESET_STM32                    (1)
#define PBDRV_CONFIG_RESET_STM32_HAS_BLE_BOOTLOADER (0)

#define PBDRV_CONFIG_UART                           (1)
#define PBDRV_CONFIG_UART_STM32F4_LL_IRQ            (1)
#define PBDRV_CONFIG_UART_STM32F4_LL_IRQ_NUM_UART   (2)

#define PBDRV_CONFIG_USB                            (1)
#define PBDRV_CONFIG_USB_STM32F4_CDC                (1)

#define PBDRV_CONFIG_WATCHDOG                       (1)
#define PBDRV_CONFIG_WATCHDOG_STM32                 (1)

#define PBDRV_CONFIG_MOTOR          (1)

#define PBDRV_CONFIG_HAS_PORT_A (1)
#define PBDRV_CONFIG_HAS_PORT_B (1)

#define PBDRV_CONFIG_FIRST_MOTOR_PORT       PBIO_PORT_ID_A
#define PBDRV_CONFIG_LAST_MOTOR_PORT        PBIO_PORT_ID_B
#define PBDRV_CONFIG_NUM_MOTOR_CONTROLLER   (2)

#define PBDRV_CONFIG_SYS_CLOCK_RATE 96000000

#endif // _PBDRVCONFIG_H_