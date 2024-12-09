// SPDX-License-Identifier: MIT
// Copyright (c) 2024 Lars Görner

#ifndef INC_CONSOLE_H_
#define INC_CONSOLE_H_

#include <stdint.h>

void console_init(void);
void console_start(void);
void console_write_str(const char * str);
void console_write_u8(uint8_t num);
void console_write_u16(uint16_t num);
void console_stop(void);

#endif // INC_CONSOLE_H_