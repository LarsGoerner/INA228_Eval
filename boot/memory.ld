/* SPDX-License-Identifier: MIT */
/* Copyright (c) 2024 Lars Görner */

MEMORY
{
        FLASH (xr): ORIGIN = 0x08000000, LENGTH = 16K
        SRAM  (rw): ORIGIN = 0x20000000, LENGTH = 4K
}

_estack = ORIGIN(SRAM) + LENGTH(SRAM);

SECTIONS
{
        .isr_vector :
        {
                KEEP(*(.isr_vector))
                . = ALIGN(4);
        } > FLASH

        .text :
        {
                . = ALIGN(4);
                *(.text)
                . = ALIGN(4);
        } > FLASH

        .rodata :
        {
                . = ALIGN(4);
                *(rodata)
                . = ALIGN(4);
        } > FLASH

        .data :
        {
                . = ALIGN(4);
                _sdata = .;
                *(.data)
                . = ALIGN(4);
                _edata = .;
        } > SRAM AT > FLASH
        _sidata = LOADADDR(.data);

        .bss :
        {
                . = ALIGN(4);
                _sbss = .;
                *(.bss)
                . = ALIGN(4);
                _ebss = .;
        } > SRAM
}
