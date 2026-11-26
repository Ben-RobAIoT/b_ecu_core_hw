/* Author: Beniot Phan (Phan Bao Tin) */
/* Bare-Metal USART1 Logger & DWT Cycle Counter Implementation */

#include "bsp_uart_dwt.h"

#define RCC_AHB1ENR_GPIOAEN_Msk     (1UL << 0U)
#define RCC_APB2ENR_USART1EN_Msk    (1UL << 4U)

#define USART_SR_TC_Msk             (1UL << 6U)
#define USART_SR_TXE_Msk            (1UL << 7U)

#define USART_CR1_RE_Msk            (1UL << 2U)
#define USART_CR1_TE_Msk            (1UL << 3U)
#define USART_CR1_UE_Msk            (1UL << 13U)

/* Hàm hook chuẩn của thư viện C (newlib-nano) cho printf() */
int _write(int file, char *ptr, int len);

void bsp_uart1_init_115200_hsi16(void)
{
    /* 1. Cấp clock cho GPIOA (AHB1) và USART1 (APB2) */
    RCC->AHB1ENR  |= RCC_AHB1ENR_GPIOAEN_Msk;
    RCC->APB2ENR  |= RCC_APB2ENR_USART1EN_Msk;

    volatile uint32_t dummy = RCC->APB2ENR;
    (void)dummy;

    /* 2. Cấu hình PA9 (TX) và PA10 (RX) sang chế độ Alternate Function (0b10) */
    GPIOA->MODER &= ~((3UL << (9U * 2U)) | (3UL << (10U * 2U)));
    GPIOA->MODER |=  ((2UL << (9U * 2U)) | (2UL << (10U * 2U)));

    /* 3. Tốc độ chuyển mạch High Speed (0b10) cho PA9, PA10 */
    GPIOA->OSPEEDR &= ~((3UL << (9U * 2U)) | (3UL << (10U * 2U)));
    GPIOA->OSPEEDR |=  ((2UL << (9U * 2U)) | (2UL << (10U * 2U)));

    /* 4. Kéo lên nội (Pull-Up = 0b01) cho PA10 (RX) tránh nhiễu framing khi thả nổi dây */
    GPIOA->PUPDR &= ~((3UL << (9U * 2U)) | (3UL << (10U * 2U)));
    GPIOA->PUPDR |=  (1UL << (10U * 2U));

    /* 5. Chọn AF7 (USART1) trong thanh ghi AFR[1] (AFRH quản lý pin 8..15)
     *    PA9 -> bit [7:4] = 0x7 | PA10 -> bit [11:8] = 0x7 */
    GPIOA->AFR[1] &= ~((0xFUL << 4U) | (0xFUL << 8U));
    GPIOA->AFR[1] |=  ((7UL   << 4U) | (7UL   << 8U));

    /* 6. Tắt USART1 trước khi cấu hình baudrate */
    USART1->CR1 = 0UL;
    USART1->CR2 = 0UL;
    USART1->CR3 = 0UL;

    /* 7. Nạp BRR = 0x008B (115200 bps tại APB2 = 16 MHz HSI, OVER8 = 0) */
    USART1->BRR = (8UL << 4U) | 11UL;

    /* 8. Bật Transmitter, Receiver và kích hoạt USART1 */
    USART1->CR1 = USART_CR1_TE_Msk | USART_CR1_RE_Msk | USART_CR1_UE_Msk;
}

void bsp_uart1_putc(char c)
{
    while ((USART1->SR & USART_SR_TXE_Msk) == 0UL) {
        /* Chờ thanh ghi truyền DR trống */
    }
    USART1->DR = (uint8_t)c;
}

int _write(int file, char *ptr, int len)
{
    (void)file;

    if ((ptr == 0) || (len <= 0)) {
        return 0;
    }

    for (int i = 0; i < len; ++i) {
        if (ptr[i] == '\n') {
            bsp_uart1_putc('\r');
        }
        bsp_uart1_putc(ptr[i]);
    }

    /* Đợi byte cuối cùng dịch xong ra khỏi shift register (TC = 1) */
    while ((USART1->SR & USART_SR_TC_Msk) == 0UL) {
    }

    return len;
}

void bsp_dwt_init(void)
{
    /* 1. Bật khối Debug & Trace toàn cục trong CoreDebug->DEMCR */
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;

    /* 2. Đặt lại bộ đếm chu kỳ về 0 */
    DWT->CYCCNT = 0UL;

    /* 3. Kích hoạt bộ đếm chu kỳ CPU (đếm 1 đơn vị mỗi chu kỳ SYSCLK) */
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}