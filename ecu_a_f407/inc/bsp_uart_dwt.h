/* Author: Beniot Phan (Phan Bao Tin) */
/* Bare-Metal USART1 Logger & ARM Cortex-M4 DWT Cycle Counter Driver */

#ifndef BSP_UART_DWT_H
#define BSP_UART_DWT_H

#include <stdint.h>
#include "stm32f407_regs.h"

void bsp_uart1_init_115200_hsi16(void);
void bsp_uart1_putc(char c);

void bsp_dwt_init(void);

static inline uint32_t bsp_dwt_get_cycles(void)
{
    return DWT->CYCCNT;
}

#endif /* BSP_UART_DWT_H */