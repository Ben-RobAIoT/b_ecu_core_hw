/* Author: Beniot Phan (Phan Bao Tin) */
/* Bare-Metal Register Definitions for STM32F407VET6 (RM0090 & ARMv7-M) */

#ifndef STM32F407_REGS_H
#define STM32F407_REGS_H

#include <stdint.h>

#define __IO volatile
#define __I  volatile const

/* ============================================================================
 * 1. ARM Cortex-M4 Core Peripherals (0xE0000000)
 * ============================================================================ */
typedef struct {
    __I  uint32_t CPUID;
    __IO uint32_t ICSR;
    __IO uint32_t VTOR;
    __IO uint32_t AIRCR;
    __IO uint32_t SCR;
    __IO uint32_t CCR;
    __IO uint8_t  SHP[12];
    __IO uint32_t SHCSR;
    __IO uint32_t CFSR;
    __IO uint32_t HFSR;
    __IO uint32_t DFSR;
    __IO uint32_t MMFAR;
    __IO uint32_t BFAR;
    __IO uint32_t AFSR;
    uint32_t      RESERVED0[18];
    __IO uint32_t CPACR;
} SCB_TypeDef;

typedef struct {
    __IO uint32_t DHCSR;
    __IO uint32_t DCRSR;
    __IO uint32_t DCRDR;
    __IO uint32_t DEMCR;
} CoreDebug_TypeDef;

typedef struct {
    __IO uint32_t CTRL;
    __IO uint32_t CYCCNT;
} DWT_TypeDef;

#define SCB_BASE        (0xE000ED00UL)
#define CoreDebug_BASE  (0xE000EDF0UL)
#define DWT_BASE        (0xE0001000UL)

#define SCB             ((SCB_TypeDef *)SCB_BASE)
#define CoreDebug       ((CoreDebug_TypeDef *)CoreDebug_BASE)
#define DWT             ((DWT_TypeDef *)DWT_BASE)

#define CoreDebug_DEMCR_TRCENA_Msk  (1UL << 24U)
#define DWT_CTRL_CYCCNTENA_Msk      (1UL << 0U)
#define SCB_CCR_DIV_0_TRP_Msk       (1UL << 4U)
#define SCB_SHCSR_USGFAULTENA_Msk   (1UL << 18U)
#define SCB_SHCSR_BUSFAULTENA_Msk   (1UL << 17U)
#define SCB_SHCSR_MEMFAULTENA_Msk   (1UL << 16U)

/* ============================================================================
 * 2. STM32F407 Peripheral Memory Map (RM0090 Table 1)
 * ============================================================================ */
#define PERIPH_BASE         (0x40000000UL)
#define APB1PERIPH_BASE     (PERIPH_BASE)
#define APB2PERIPH_BASE     (PERIPH_BASE + 0x00010000UL)
#define AHB1PERIPH_BASE     (PERIPH_BASE + 0x00020000UL)

#define GPIOA_BASE          (AHB1PERIPH_BASE + 0x0000UL)
#define GPIOB_BASE          (AHB1PERIPH_BASE + 0x0400UL)
#define GPIOC_BASE          (AHB1PERIPH_BASE + 0x0800UL)
#define GPIOD_BASE          (AHB1PERIPH_BASE + 0x0C00UL)
#define GPIOE_BASE          (AHB1PERIPH_BASE + 0x1000UL)
#define RCC_BASE            (AHB1PERIPH_BASE + 0x3800UL)
#define USART1_BASE         (APB2PERIPH_BASE + 0x1000UL)

/* ============================================================================
 * 3. RCC, GPIO, USART Register Structures
 * ============================================================================ */
typedef struct {
    __IO uint32_t CR;
    __IO uint32_t PLLCFGR;
    __IO uint32_t CFGR;
    __IO uint32_t CIR;
    __IO uint32_t AHB1RSTR;
    __IO uint32_t AHB2RSTR;
    __IO uint32_t AHB3RSTR;
    uint32_t      RESERVED0;
    __IO uint32_t APB1RSTR;
    __IO uint32_t APB2RSTR;
    uint32_t      RESERVED1[2];
    __IO uint32_t AHB1ENR;
    __IO uint32_t AHB2ENR;
    __IO uint32_t AHB3ENR;
    uint32_t      RESERVED2;
    __IO uint32_t APB1ENR;
    __IO uint32_t APB2ENR;
    uint32_t      RESERVED3[2];
    __IO uint32_t AHB1LPENR;
    __IO uint32_t AHB2LPENR;
    __IO uint32_t AHB3LPENR;
    uint32_t      RESERVED4;
    __IO uint32_t APB1LPENR;
    __IO uint32_t APB2LPENR;
    uint32_t      RESERVED5[2];
    __IO uint32_t BDCR;
    __IO uint32_t CSR;
    uint32_t      RESERVED6[2];
    __IO uint32_t SSCGR;
    __IO uint32_t PLLI2SCFGR;
} RCC_TypeDef;

typedef struct {
    __IO uint32_t MODER;
    __IO uint32_t OTYPER;
    __IO uint32_t OSPEEDR;
    __IO uint32_t PUPDR;
    __IO uint32_t IDR;
    __IO uint32_t ODR;
    __IO uint32_t BSRR;
    __IO uint32_t LCKR;
    __IO uint32_t AFR[2];
} GPIO_TypeDef;

typedef struct {
    __IO uint32_t SR;
    __IO uint32_t DR;
    __IO uint32_t BRR;
    __IO uint32_t CR1;
    __IO uint32_t CR2;
    __IO uint32_t CR3;
    __IO uint32_t GTPR;
} USART_TypeDef;

#define RCC     ((RCC_TypeDef *)RCC_BASE)
#define GPIOA   ((GPIO_TypeDef *)GPIOA_BASE)
#define GPIOB   ((GPIO_TypeDef *)GPIOB_BASE)
#define GPIOC   ((GPIO_TypeDef *)GPIOC_BASE)
#define GPIOD   ((GPIO_TypeDef *)GPIOD_BASE)
#define GPIOE   ((GPIO_TypeDef *)GPIOE_BASE)
#define USART1  ((USART_TypeDef *)USART1_BASE)

#endif /* STM32F407_REGS_H */