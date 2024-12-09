# SPDX-License-Identifier: MIT
# Copyright (c) 2024 Lars Görner

TC_PREFIX := arm-none-eabi
CC := $(TC_PREFIX)-gcc.exe
OBJ_DUMP := $(TC_PREFIX)-objdump.exe

FW_FILE := build/fw.elf
FW_FILE_DUMP := $(subst .elf,.list,$(FW_FILE))
LD_SCRIPT := boot/memory.ld

CFLAGS += -mcpu=cortex-m0
CFLAGS += -Og -g
#CFLAGS += -Os
CFLAGS += -Iinc
CFLAGS += -DSTM32F030x6
CFLAGS += -std=c11 -ffreestanding -fno-builtin
CFLAGS += -Wall -Wextra -Wpedantic
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -Wl,--nostdlib,--gc-sections,--print-memory-usage,-T,$(LD_SCRIPT)

SRC := \
	boot/startup_stm32f030x6.s \
	src/main.c \
	src/hw_init.c \
	src/console.c \
	src/ina228.c

.PHONY: all clean ocd

all: $(SRC)
	$(CC) $(CFLAGS) -o $(FW_FILE) $^
	$(OBJ_DUMP) --disassemble --source --headers $(FW_FILE) > $(FW_FILE_DUMP)

clean:
	rm build/*

ocd:
	openocd -f interface/stlink.cfg -f target/stm32f0x.cfg
