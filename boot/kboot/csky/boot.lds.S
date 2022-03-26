/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <size.h>
#include <asm-generic/kernel.lds.h>

OUTPUT_ARCH(csky)
ENTRY(_start)

#define STACK_SIZE  (SZ_1KiB * 8)
#define HEAP_SIZE   (SZ_1KiB * 128)

SECTIONS {
    . = CONFIG_LOAD_ADDR;

    .startup : {
        KEEP(*(.startup))
        KEEP(*(.startup.*))
    }

    .text : {
        . = ALIGN(4);
        *(.text)
        *(.text.*)
    }

    .rodata : {
        . = ALIGN(4);
        *(.rodata)
        *(.rodata.*)
    }

    .got : {
        . = ALIGN(4);
        _ld_got_start = .;
        *(.got)
        *(.got*)
        . = ALIGN(4);
        _ld_got_end = .;
    }

    .data : {
        . = ALIGN(4);
        *(.data)
        *(.data.*)
    }

    .piggydata : {
        _ld_piggy_start = .;
        KEEP(*(.piggydata))
        _ld_piggy_end = .;
    }

    .bss (NOLOAD) : {
        . = ALIGN(4);
        _ld_bss_start = .;
        *(.bss)
        *(.bss.*)
        *(COMMON)
        . = ALIGN(4);
        _ld_bss_end = .;
    }

    .stack (NOLOAD) : {
        . = ALIGN(4);
        _ld_stack_start = .;
        . = _ld_stack_start + STACK_SIZE;
        _ld_stack_end = .;
    }

    .heap (NOLOAD) : {
        . = ALIGN(4);
        _ld_heap_start = .;
        . = _ld_heap_start + HEAP_SIZE;
        . = ALIGN(4);
        _ld_heap_end = .;
    }

    STABS_DEBUG
    DWARF_DEBUG
    ELF_DETAILS
    DISCARDS

    /DISCARD/ : {
        *(.dynamic) *(.dynsym) *(.dynstr) *(.dynbss)
        *(.hash) *(.gnu.hash)
        *(.note.*)
    }
}
