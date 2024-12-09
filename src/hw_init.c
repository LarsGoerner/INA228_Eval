// SPDX-License-Identifier: MIT
// Copyright (c) 2024 Lars Görner

#include <stdint.h>
#include <system/stm32f0xx.h>
#include <hw_init.h>

#define GPIO_MODE_INPUT         0
#define GPIO_MODE_OUTPUT        1
#define GPIO_MODE_ALTF          2
#define GPIO_MODE_ANALOG        3

static void rcc_init(void)
{
        SET_BIT(RCC->AHBENR, RCC_AHBENR_GPIOAEN);
        SET_BIT(RCC->APB1ENR, RCC_APB1ENR_I2C1EN);
        SET_BIT(RCC->APB2ENR, RCC_APB2ENR_USART1EN);
}

static void gpio_init(void)
{
        //---| PA
        // PA0  - X
        // PA1  - X
        // PA2  - USART
        // PA3  - USART
        // PA4  - X
        // PA5  - X
        // PA6  - X
        // PA7  - ALARM
        // PA8  - X
        // PA9  - I2C
        // PA10 - I2C
        // PA11 - X
        // PA12 - X
        // PA13 - SWDIO
        // PA14 - SWCLK
        // PA15 - X
        GPIOA->MODER =
                (GPIO_MODE_ANALOG << GPIO_MODER_MODER0_Pos) |
                (GPIO_MODE_ANALOG << GPIO_MODER_MODER1_Pos) |
                (GPIO_MODE_ALTF << GPIO_MODER_MODER2_Pos) |
                (GPIO_MODE_ALTF << GPIO_MODER_MODER3_Pos) |
                (GPIO_MODE_ANALOG << GPIO_MODER_MODER4_Pos) |
                (GPIO_MODE_ANALOG << GPIO_MODER_MODER5_Pos) |
                (GPIO_MODE_ANALOG << GPIO_MODER_MODER6_Pos) |
                (GPIO_MODE_INPUT << GPIO_MODER_MODER7_Pos) |
                (GPIO_MODE_ANALOG << GPIO_MODER_MODER8_Pos) |
                (GPIO_MODE_ALTF << GPIO_MODER_MODER9_Pos) |
                (GPIO_MODE_ALTF << GPIO_MODER_MODER10_Pos) |
                (GPIO_MODE_ANALOG << GPIO_MODER_MODER11_Pos) |
                (GPIO_MODE_ANALOG << GPIO_MODER_MODER12_Pos) |
                (GPIO_MODE_ALTF << GPIO_MODER_MODER13_Pos) |
                (GPIO_MODE_ALTF << GPIO_MODER_MODER14_Pos) |
                (GPIO_MODE_ANALOG << GPIO_MODER_MODER15_Pos);
        GPIOA->OTYPER =
                GPIO_OTYPER_OT_9 | // I2C pin -> open-drain
                GPIO_OTYPER_OT_10; // I2C pin -> open-drain
        GPIOA->OSPEEDR =
                GPIO_OSPEEDER_OSPEEDR2 |
                GPIO_OSPEEDER_OSPEEDR3 |
                GPIO_OSPEEDER_OSPEEDR9 |
                GPIO_OSPEEDER_OSPEEDR10 |
                GPIO_OSPEEDER_OSPEEDR13 |
                GPIO_OSPEEDER_OSPEEDR14;
        GPIOA->PUPDR =
                GPIO_PUPDR_PUPDR9_0 | // I2C pin -> pull-up
                GPIO_PUPDR_PUPDR10_0; // I2C pin -> pull-up
        GPIOA->AFR[0] =
                ((1 << GPIO_AFRL_AFRL2_Pos) & GPIO_AFRL_AFRL2_Msk) |
                ((1 << GPIO_AFRL_AFRL3_Pos) & GPIO_AFRL_AFRL3_Msk);
        GPIOA->AFR[1] =
                ((4 << GPIO_AFRH_AFSEL9_Pos) & GPIO_AFRH_AFSEL9_Msk) |
                ((4 << GPIO_AFRH_AFSEL10_Pos) & GPIO_AFRH_AFSEL10_Msk);
}

void hw_init(void)
{
        rcc_init();
        gpio_init();
}