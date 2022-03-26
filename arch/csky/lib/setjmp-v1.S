/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <linkage.h>
#include <asm-offset.h>

SYM_CODE_START(setjmp)
    stw     sp, (a0, SETJMP_SP)
    stw     l0, (a0, SETJMP_L0)
    stw     l1, (a0, SETJMP_L1)
    stw     l2, (a0, SETJMP_L2)
    stw     l3, (a0, SETJMP_L3)
    stw     l4, (a0, SETJMP_L4)
    stw     l5, (a0, SETJMP_L5)
    stw     gb, (a0, SETJMP_GB)
    stw     lr, (a0, SETJMP_LR)

    xor     a0, a0
    rts
SYM_CODE_END(setjmp)

SYM_CODE_START(longjmp)
    ldw     sp, (a0, SETJMP_SP)
    ldw     l0, (a0, SETJMP_L0)
    ldw     l1, (a0, SETJMP_L1)
    ldw     l2, (a0, SETJMP_L2)
    ldw     l3, (a0, SETJMP_L3)
    ldw     l4, (a0, SETJMP_L4)
    ldw     l5, (a0, SETJMP_L5)
    ldw     gb, (a0, SETJMP_GB)
    ldw     lr, (a0, SETJMP_LR)

    mov     a0, a1
    cmpnei  a1, 0
    bt      1f
    movi    a0, 1
1:
    rts
SYM_CODE_END(longjmp)
