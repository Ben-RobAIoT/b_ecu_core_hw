/* Author: Beniot Phan (Phan Bao Tin) */
/* Bare-Metal GPIO & Onboard LED Driver for STM32F407VET6 */

#ifndef BSP_GPIO_H
#define BSP_GPIO_H

#include <stdint.h>

/* Onboard LED D2 trên STM32F407VET6 nối tại PA6 (Active-LOW) */
#define BSP_LED_PORT        GPIOA
#define BSP_LED_PIN         (6U)

void bsp_gpio_init(void);
void bsp_led_on(void);
void bsp_led_off(void);
void bsp_led_toggle(void);

#endif /* BSP_GPIO_H */