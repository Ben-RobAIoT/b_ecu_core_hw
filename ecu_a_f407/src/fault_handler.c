/* Author: Beniot Phan (Phan Bao Tin) */
/* Cortex-M4 HardFault Naked Entry, Stack Unwinder & .noinit Crash Logger */

#include <stdio.h>
#include "fault_handler.h"
#include "stm32f407_regs.h"
#include "bsp_gpio.h"

#define SRAM_START_ADDR     (0x20000000UL)
#define SRAM_END_ADDR       (0x20020000UL)

/* Đặt biến crash log vào section .noinit (không bị xóa khi warm reset) */
__attribute__((section(".noinit"))) static crash_log_t s_crash_log;

void fault_handler_init(void)
{
    /* Bật bẫy chia cho 0 (DIV_0_TRP) để kiểm chứng UsageFault ở Ca 3 */
    SCB->CCR |= SCB_CCR_DIV_0_TRP_Msk;
}

void fault_check_and_report_previous_crash(void)
{
    if (s_crash_log.magic == CRASH_LOG_MAGIC) {
        printf("\r\n[ECU-A][BOOT-DIAG] Previous Crash Recovered from .noinit!\r\n");
        printf("  PC=0x%08lX | LR=0x%08lX | CFSR=0x%08lX | BFAR=0x%08lX\r\n",
               (unsigned long)s_crash_log.pc,
               (unsigned long)s_crash_log.lr,
               (unsigned long)s_crash_log.cfsr,
               (unsigned long)s_crash_log.bfar);

        /* Xóa magic sau khi đã báo cáo để không lặp lại ở lần boot sau */
        s_crash_log.magic = 0UL;
    }
}

__attribute__((naked)) void HardFault_Handler(void)
{
    __asm volatile (
        "tst   lr, #4               \n" /* Kiểm tra bit 2 của EXC_RETURN */
        "ite   eq                   \n" /* If-Then-Else Thumb-2 */
        "mrseq r0, msp              \n" /* Bit 2 == 0 -> lấy MSP */
        "mrsne r0, psp              \n" /* Bit 2 == 1 -> lấy PSP */
        "mov   r1, lr               \n" /* Tham số 2: giá trị EXC_RETURN */
        "b     HardFault_Handler_C  \n" /* Nhảy sang hàm C phân tích */
    );
}

void HardFault_Handler_C(uint32_t *sp, uint32_t exc_return)
{
    /* Lưu thanh ghi hệ thống trước tiên */
    uint32_t cfsr  = SCB->CFSR;
    uint32_t hfsr  = SCB->HFSR;
    uint32_t mmfar = SCB->MMFAR;
    uint32_t bfar  = SCB->BFAR;

    uint32_t sp_addr = (uint32_t)sp;
    uint32_t r0 = 0UL, r1 = 0UL, r2 = 0UL, r3 = 0UL;
    uint32_t r12 = 0UL, lr = 0UL, pc = 0UL, xpsr = 0UL;

    /* Kiểm tra biên SRAM trước khi đọc stack frame (phòng trường hợp Stack Overflow) */
    if ((sp_addr >= SRAM_START_ADDR) && (sp_addr <= (SRAM_END_ADDR - 32UL))) {
        r0   = sp[0];
        r1   = sp[1];
        r2   = sp[2];
        r3   = sp[3];
        r12  = sp[4];
        lr   = sp[5];
        pc   = sp[6];
        xpsr = sp[7];
    }

    /* Ghi lại vào vùng .noinit */
    s_crash_log.magic      = CRASH_LOG_MAGIC;
    s_crash_log.exc_return = exc_return;
    s_crash_log.r0         = r0;
    s_crash_log.r1         = r1;
    s_crash_log.r2         = r2;
    s_crash_log.r3         = r3;
    s_crash_log.r12        = r12;
    s_crash_log.lr         = lr;
    s_crash_log.pc         = pc;
    s_crash_log.xpsr       = xpsr;
    s_crash_log.cfsr       = cfsr;
    s_crash_log.hfsr       = hfsr;
    s_crash_log.mmfar      = mmfar;
    s_crash_log.bfar       = bfar;

    /* In toàn bộ ngữ cảnh lỗi qua USART1 (dùng polling nên an toàn khi ngắt bị khóa) */
    printf("\r\n================ [HARDFAULT DETECTED] ================\r\n");
    printf("Stack Source : %s (SP = 0x%08lX, EXC_RETURN = 0x%08lX)\r\n",
           ((exc_return & 0x04UL) != 0UL) ? "PSP" : "MSP",
           (unsigned long)sp_addr,
           (unsigned long)exc_return);
    printf("R0  = 0x%08lX | R1  = 0x%08lX | R2  = 0x%08lX | R3  = 0x%08lX\r\n",
           (unsigned long)r0, (unsigned long)r1, (unsigned long)r2, (unsigned long)r3);
    printf("R12 = 0x%08lX | LR  = 0x%08lX | PC  = 0x%08lX | xPSR= 0x%08lX\r\n",
           (unsigned long)r12, (unsigned long)lr, (unsigned long)pc, (unsigned long)xpsr);
    printf("CFSR= 0x%08lX | HFSR= 0x%08lX | MMFAR=0x%08lX | BFAR= 0x%08lX\r\n",
           (unsigned long)cfsr, (unsigned long)hfsr, (unsigned long)mmfar, (unsigned long)bfar);
    printf("======================================================\r\n");

    /* Bật sáng cố định LED D2 báo trạng thái Safe/Fault và treo vòng lặp chờ debug */
    bsp_led_on();
    while (1) {
        __asm volatile ("nop");
    }
}