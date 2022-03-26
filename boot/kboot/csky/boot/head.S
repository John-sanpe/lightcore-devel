/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <linkage.h>

    .section .startup,"ax"
ENTRY(_start)
    bsr         init
END(_start)

ENTRY(init)
    psrclr      ee, fe, ie
    psrclr      af

    lrw         l0, CONFIG_LOAD_ADDR
    lrw         l1, 0xfffff000
    and         lr, l1
    sub         l0, lr

    cmpnei      l0, 0
    bf          bss_setup

got_setup:
    /* initialize preload got table */
    lrw         l1, _ld_got_start
    lrw         l2, _ld_got_end
    sub         l1, l0
    sub         l2, l0
    br          sgot_loop
1:
    ldw         r1, (l1, 0)
    sub         r1, l0
    stw         r1, (l1, 0)
    addi        l1, 4
sgot_loop:
    cmphs       l1, l2
    bf          1b

bss_setup:
    /* clear the bss region */
    xor         r1, r1
    lrw         l1, _ld_bss_start
    lrw         l2, _ld_bss_end
    sub         l1, l0
    sub         l2, l0
    br          bss_loop
1:
    stw         r1, (l1, 0)
    addi        l1, 4
bss_loop:
    cmphs       l1, l2
    bf          1b

    lrw         r1, _ld_stack_end
    sub         r1, l0
    mov         sp, r1

    psrset      ee, fe, ie
    bsr         main
    br          halt
END(init)

GLOBAL(kernel_start)
    jmp         a0
END(kernel_start)

GLOBAL(halt)
    stop
    br      halt
END(halt)
