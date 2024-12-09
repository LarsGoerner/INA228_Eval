// SPDX-License-Identifier: MIT
// Copyright (c) 2024 Lars Görner

#ifndef INC_INA228_H_
#define INC_INA228_H_

void ina228_init(void);
const char * ina228_get_manufacturer_id(void);
uint16_t ina228_get_device_id(void);
int ina228_get_current(void);
uint32_t ina228_get_voltage(void);
uint16_t ina228_get_temperature(void);
uint32_t ina228_get_power(void);
uint16_t ina228_get_energy(void);
uint16_t ina228_get_charge(void);

#endif // INC_INA228_H_