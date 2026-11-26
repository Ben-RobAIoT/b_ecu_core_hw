/* Author: Beniot Phan (Phan Bao Tin) */
/* ECU-A (STM32F407VET6) Bare-Metal Entry Point - Step 2.4 & Shift 3 Fault Experiments */

#include <stdio.h>
#include "stm32f407_regs.h"
#include "bsp_gpio.h"
#include "bsp_uart_dwt.h"
#include "fault_handler.h"
#include "fault_experiments.h"

void SystemInit(void);

void SystemInit(void)
{
    /* Cấp quyền truy cập Full (0b11) cho CP10 & CP11 (Hardware FPU) */
    SCB->CPACR |= ((3UL << (10U * 2U)) | (3UL << (11U * 2U)));
    __asm volatile ("dsb");
    __asm volatile ("isb");

    /* Cố định bảng vector ngắt tại đầu Flash */
    SCB->VTOR = 0x08000000UL;
}

static void delay_busy_cycles(volatile uint32_t cycles)
{
    while (cycles > 0UL) {
        cycles--;
    }
}

int main(void)
{
    fault_handler_init();
    bsp_gpio_init();
    bsp_uart1_init_115200_hsi16();
    bsp_dwt_init();

    printf("\r\n[ECU-A] Boot OK | SYSCLK=16MHz (HSI) | DWT_CTRL=0x%08lX\r\n",
           (unsigned long)DWT->CTRL);

    /* Kiểm tra nếu lần chạy trước vừa dính HardFault (lưu trong .noinit) */
    fault_check_and_report_previous_crash();

    /* Chạy kiểm chứng địa chỉ vùng nhớ & struct padding */
    exp_verify_memory_and_padding();

    /* Kích hoạt kịch bản tiêm lỗi nếu FAULT_INJECT_SCENARIO != 0 */
    exp_run_hardfault_injection(FAULT_INJECT_SCENARIO);

    uint32_t tick = 0UL;

    while (1) {
        uint32_t t0 = bsp_dwt_get_cycles();
        bsp_led_toggle();
        uint32_t t1 = bsp_dwt_get_cycles();

        delay_busy_cycles(800000UL);
        uint32_t t2 = bsp_dwt_get_cycles();

        printf("[ECU-A] tick=%lu | toggle=%lu cyc | period=%lu cyc\r\n",
               (unsigned long)tick,
               (unsigned long)(t1 - t0),
               (unsigned long)(t2 - t0));
        tick++;
    }

    return 0;
}