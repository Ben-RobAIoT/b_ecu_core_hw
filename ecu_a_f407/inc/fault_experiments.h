/* Author: Beniot Phan (Phan Bao Tin) */
/* Day 1 Shift 3: Fault Injection & Memory Layout Experiments */

#ifndef FAULT_EXPERIMENTS_H
#define FAULT_EXPERIMENTS_H

#include <stdint.h>

/* Chọn kịch bản tiêm lỗi HardFault ở Thí nghiệm 3:
 * 0: Chạy bình thường (không tiêm lỗi)
 * 1: BusFault (ghi vào vùng nhớ Reserved 0x30000000 -> PRECISERR + BFAR)
 * 2: UsageFault (chia cho 0 với DIV_0_TRP -> DIVBYZERO)
 * 3: UsageFault (nhảy vào con trỏ hàm địa chỉ chẵn thiếu Thumb bit -> INVSTATE)
 */
#define FAULT_INJECT_SCENARIO   (0U)

void exp_verify_memory_and_padding(void);
void exp_run_hardfault_injection(uint32_t scenario);

#endif /* FAULT_EXPERIMENTS_H */