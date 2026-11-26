/* Author: Beniot Phan (Phan Bao Tin) */
/* Cortex-M4 HardFault Decoder & Persistent .noinit Crash Log */

#ifndef FAULT_HANDLER_H
#define FAULT_HANDLER_H

#include <stdint.h>

#define CRASH_LOG_MAGIC     (0xDEAD10C0UL)

typedef struct {
    uint32_t magic;
    uint32_t exc_return;
    uint32_t r0;
    uint32_t r1;
    uint32_t r2;
    uint32_t r3;
    uint32_t r12;
    uint32_t lr;
    uint32_t pc;
    uint32_t xpsr;
    uint32_t cfsr;
    uint32_t hfsr;
    uint32_t mmfar;
    uint32_t bfar;
} crash_log_t;

void fault_handler_init(void);
void fault_check_and_report_previous_crash(void);
void HardFault_Handler_C(uint32_t *sp, uint32_t exc_return);

#endif /* FAULT_HANDLER_H */