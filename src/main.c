// SPDX-License-Identifier: MIT
// Copyright (c) 2024 Lars Görner

#include <hw_init.h>
#include <console.h>
#include <ina228.h>

void main(void)
{
        hw_init();
        console_init();
        ina228_init();

        console_start();
        console_write_str("Test num: ");
        console_write_u8(0);
        console_write_str("!\n\rTest end!!!\n\r");
        console_stop();

        console_start();
        console_write_str("INA228 Test:\n\r");
        console_write_str("Manufacturer ID (should 'TI'): ");
        console_write_str(ina228_get_manufacturer_id());
        console_write_str("\n\r");
        console_stop();

        console_start();
        while (1) {
                int current = ina228_get_current();
                uint16_t cval = current >= 0 ? current : current * (-1);
                uint16_t vbus = ina228_get_voltage();
                console_write_str("Current = ");
                if (current < 0) {
                        console_write_str("-");
                }
                console_write_u16(cval);
                console_write_str("\n\r");
                console_write_str("Bus Voltage = ");
                console_write_u16(vbus);
                console_write_str("\n\r");
        }
        console_stop();
}
