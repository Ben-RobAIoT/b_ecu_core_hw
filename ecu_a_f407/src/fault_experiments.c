/* Author: Beniot Phan (Phan Bao Tin) */
/* Implementation of Memory Layout, Padding & HardFault Injection Experiments */

#include <stdio.h>
#include <stddef.h>
#include "fault_experiments.h"
#include "stm32f407_regs.h"

/* ============================================================================
 * Thí nghiệm 2: Phân bố biến trên các vùng nhớ (.rodata, .data, .bss, .ccmram)
 * ============================================================================ */
const uint32_t g_exp_rodata_const = 0x20261126UL;          /* Flash: .rodata */
uint32_t       g_exp_data_init    = 0xCAFEBABEUL;          /* RAM (.data), LMA ở Flash */
uint32_t       g_exp_bss_zero;                             /* RAM (.bss) */
__attribute__((section(".ccmram")))
uint8_t        g_exp_ccm_buffer[64];                       /* CCMRAM: 0x10000000 (CPU-only) */

/* So sánh Struct chưa tối ưu thứ tự vs Natural Alignment (0-padding) */
typedef struct {
    uint8_t  id;       /* Offset 0 (+ 3 bytes padding) */
    uint32_t value;    /* Offset 4..7 */
    uint8_t  status;   /* Offset 8 (+ 3 bytes tail padding) -> Tổng 12 bytes */
} unaligned_bad_struct_t;

typedef struct {
    uint32_t value;    /* Offset 0..3 */
    uint8_t  id;       /* Offset 4 */
    uint8_t  status;   /* Offset 5 (+ 2 bytes tail padding) -> Tổng 8 bytes */
} aligned_good_struct_t;

_Static_assert(sizeof(unaligned_bad_struct_t) == 12U, "Expected 12B with padding");
_Static_assert(sizeof(aligned_good_struct_t)  == 8U,  "Expected 8B naturally aligned");

void exp_verify_memory_and_padding(void)
{
    g_exp_ccm_buffer[0] = 0xA5U;

    printf("\r\n[EXP-2] Memory Map Addresses:\r\n");
    printf("  .rodata (Flash) : 0x%08lX (val=0x%08lX)\r\n",
           (unsigned long)(uintptr_t)&g_exp_rodata_const,
           (unsigned long)g_exp_rodata_const);
    printf("  .data   (SRAM)  : 0x%08lX (val=0x%08lX)\r\n",
           (unsigned long)(uintptr_t)&g_exp_data_init,
           (unsigned long)g_exp_data_init);
    printf("  .bss    (SRAM)  : 0x%08lX (val=0x%08lX)\r\n",
           (unsigned long)(uintptr_t)&g_exp_bss_zero,
           (unsigned long)g_exp_bss_zero);
    printf("  .ccmram (CCM)   : 0x%08lX (val=0x%02X)\r\n",
           (unsigned long)(uintptr_t)g_exp_ccm_buffer,
           (unsigned int)g_exp_ccm_buffer[0]);

    printf("[EXP-2] Struct Padding Check:\r\n");
    printf("  unaligned_bad_struct_t  sizeof=%u (offset value=%u, status=%u)\r\n",
           (unsigned int)sizeof(unaligned_bad_struct_t),
           (unsigned int)offsetof(unaligned_bad_struct_t, value),
           (unsigned int)offsetof(unaligned_bad_struct_t, status));
    printf("  aligned_good_struct_t   sizeof=%u (offset id=%u, status=%u)\r\n",
           (unsigned int)sizeof(aligned_good_struct_t),
           (unsigned int)offsetof(aligned_good_struct_t, id),
           (unsigned int)offsetof(aligned_good_struct_t, status));
}

/* ============================================================================
 * Thí nghiệm 3: Tự tiêm lỗi kích hoạt HardFault (BusFault / UsageFault)
 * ============================================================================ */
void exp_run_hardfault_injection(uint32_t scenario)
{
    if (scenario == 0U) {
        return;
    }

    printf("\r\n[EXP-3] Triggering HardFault Scenario #%lu...\r\n", (unsigned long)scenario);

    if (scenario == 1U) {
        /* Kịch bản 1: Ghi vào vùng nhớ Reserved (0x30000000 không ánh xạ ngoại vi/RAM) */
        volatile uint32_t *reserved_ptr = (volatile uint32_t *)0x30000000UL;
        *reserved_ptr = 0x12345678UL;
    } else if (scenario == 2U) {
        /* Kịch bản 2: Chia cho 0 (khi SCB->CCR.DIV_0_TRP = 1) */
        volatile uint32_t dividend = 1000UL;
        volatile uint32_t divisor  = 0UL;
        volatile uint32_t result   = dividend / divisor;
        (void)result;
    } else if (scenario == 3U) {
        /* Kịch bản 3: Gọi con trỏ hàm có bit 0 = 0 (thiếu Thumb bit -> INVSTATE) */
        typedef void (*func_t)(void);
        func_t invalid_thumb_fn = (func_t)(uintptr_t)0x08000200UL;
        invalid_thumb_fn();
    }
}