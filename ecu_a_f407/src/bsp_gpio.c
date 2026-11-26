/* Author: Beniot Phan (Phan Bao Tin) */
/* Bare-Metal GPIO & Onboard LED Implementation (Pure Register Access) */

#include "bsp_gpio.h"
#include "stm32f407_regs.h"

#define RCC_AHB1ENR_GPIOAEN_Msk     (1UL << 0U)

void bsp_gpio_init(void)
{
    /* 1. Cấp clock cho GPIOA trên bus AHB1 */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN_Msk;

    /* Dummy read sau khi bật clock RCC (RM0090): đảm bảo bus AHB1 ổn định
     * sau 2 chu kỳ clock trước khi truy cập thanh ghi ngoại vi */
    volatile uint32_t dummy = RCC->AHB1ENR;
    (void)dummy;

    /* 2. Đặt trạng thái mặc định LED TẮT (PA6 = HIGH do Active-LOW) trước khi xuất */
    bsp_led_off();

    /* 3. Cấu hình PA6 là General Purpose Output (MODER[13:12] = 0b01) */
    BSP_LED_PORT->MODER &= ~(3UL << (BSP_LED_PIN * 2U));
    BSP_LED_PORT->MODER |=  (1UL << (BSP_LED_PIN * 2U));

    /* 4. Kiểu ngõ ra Push-Pull (OTYPER bit 6 = 0) */
    BSP_LED_PORT->OTYPER &= ~(1UL << BSP_LED_PIN);

    /* 5. Tốc độ chuyển mạch Low Speed (OSPEEDR[13:12] = 0b00) giảm nhiễu EMI */
    BSP_LED_PORT->OSPEEDR &= ~(3UL << (BSP_LED_PIN * 2U));

    /* 6. Không dùng điện trở kéo nội (PUPDR[13:12] = 0b00) */
    BSP_LED_PORT->PUPDR &= ~(3UL << (BSP_LED_PIN * 2U));
}

void bsp_led_on(void)
{
    /* Active-LOW: Ghi 1 vào bit (PIN + 16) của BSRR để kéo PA6 xuống LOW nguyên tử */
    BSP_LED_PORT->BSRR = (1UL << (BSP_LED_PIN + 16U));
}

void bsp_led_off(void)
{
    /* Active-LOW: Ghi 1 vào bit (PIN) của BSRR để kéo PA6 lên HIGH nguyên tử */
    BSP_LED_PORT->BSRR = (1UL << BSP_LED_PIN);
}

void bsp_led_toggle(void)
{
    /* Đọc trạng thái thực tế của chân từ IDR và đảo mức bằng thanh ghi BSRR */
    if ((BSP_LED_PORT->IDR & (1UL << BSP_LED_PIN)) != 0UL) {
        bsp_led_on();
    } else {
        bsp_led_off();
    }
}