// SPDX-License-Identifier: MIT
// Copyright (c) 2024 Lars Görner

#include <stdint.h>
#include <system/stm32f0xx.h>
#include <hw_defs.h>
#include <console.h>

#define CLS_BUS         USART1

#define BAUD_RATE       115200
#define USART_DIV       (MCU_FREQ / BAUD_RATE)

static char num_str_buf[6];

__STATIC_INLINE uint32_t console_tx_ready(void)
{
        return READ_BIT(CLS_BUS->ISR, USART_ISR_TXE);
}

__STATIC_INLINE uint32_t console_rx_ready(void)
{
        return READ_BIT(CLS_BUS->ISR, USART_ISR_RXNE);
}

__STATIC_INLINE void console_wait_tx_done(void)
{
        while (!console_tx_ready());
        while (!READ_BIT(CLS_BUS->ISR, USART_ISR_TC));
}

void console_init(void)
{
        // config uart
        CLEAR_BIT(CLS_BUS->CR1, USART_CR1_M); // 1 start bit, 8 data bits
        CLEAR_BIT(CLS_BUS->CR1, USART_CR1_OVER8); // oversampling by 16
        WRITE_REG(CLS_BUS->BRR, USART_DIV);
        CLEAR_BIT(CLS_BUS->CR2, USART_CR2_STOP); // 1 stop bit
}

void console_start(void)
{
        SET_BIT(CLS_BUS->CR1, USART_CR1_UE);
        SET_BIT(CLS_BUS->CR1, USART_CR1_TE | USART_CR1_RE);
}

void console_write_str(const char * str)
{
        while (*str != '\0') {
                while (!console_tx_ready());
                WRITE_REG(CLS_BUS->TDR, *str++);
        }
}

void console_write_u8(uint8_t num)
{
        register unsigned str_s_idx;

        // 100s
        num_str_buf[0] = '0';
        while (num >= 100) {
                num_str_buf[0]++;
                num -= 100;
        }

        // 10s
        num_str_buf[1] = '0';
        while (num >= 10) {
                num_str_buf[1]++;
                num -= 10;
        }

        // 1s
        num_str_buf[2] = '0' + num;
        num_str_buf[3] = '\0';

        // skip leading '0's
        for (str_s_idx = 0; str_s_idx < 2; str_s_idx++) {
                if (num_str_buf[str_s_idx] != '0') {
                        break;
                }
        }
        console_write_str(&num_str_buf[str_s_idx]);
}

void console_write_u16(uint16_t num)
{
        register unsigned str_s_idx;

        // 10000s
        num_str_buf[0] = '0';
        while (num >= 10000) {
                num_str_buf[0]++;
                num -= 10000;
        }

        // 1000s
        num_str_buf[1] = '0';
        while (num >= 1000) {
                num_str_buf[1]++;
                num -= 1000;
        }

        // 100s
        num_str_buf[2] = '0';
        while (num >= 100) {
                num_str_buf[2]++;
                num -= 100;
        }

        // 10s
        num_str_buf[3] = '0';
        while (num >= 10) {
                num_str_buf[3]++;
                num -= 10;
        }

        // 1s
        num_str_buf[4] = '0' + num;
        num_str_buf[5] = '\0';

        // skip leading '0's
        for (str_s_idx = 0; str_s_idx < 4; str_s_idx++) {
                if (num_str_buf[str_s_idx] != '0') {
                        break;
                }
        }
        console_write_str(&num_str_buf[str_s_idx]);
}

void console_stop(void)
{
        console_wait_tx_done();
        CLEAR_BIT(CLS_BUS->CR1, USART_CR1_UE);
}
