/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <linkage.h>
#include <asm-offset.h>
#include <asm/bug.h>

SYM_CODE_START(getcontext)
    /* save the preserved register values */
    stw     sp, (a0, SETJMP_SP)
    stw     l0, (a0, SETJMP_L0)
    stw     l1, (a0, SETJMP_L1)
    stw     l2, (a0, SETJMP_L2)
    stw     l3, (a0, SETJMP_L3)
    stw     l4, (a0, SETJMP_L4)
    stw     l5, (a0, SETJMP_L5)
    stw     gb, (a0, SETJMP_GB)
    stw     lr, (a0, SETJMP_LR)

    rts
SYM_CODE_END(getcontext)

SYM_CODE_START(setcontext)
    /* load the values of all the preserved registers */
    ldw     sp, (a0, SETJMP_SP)
    ldw     l0, (a0, SETJMP_L0)
    ldw     l1, (a0, SETJMP_L1)
    ldw     l2, (a0, SETJMP_L2)
    ldw     l3, (a0, SETJMP_L3)
    ldw     l4, (a0, SETJMP_L4)
    ldw     l5, (a0, SETJMP_L5)
    ldw     gb, (a0, SETJMP_GB)
    ldw     lr, (a0, SETJMP_LR)

    jmp     lr
SYM_CODE_END(setcontext)

SYM_CODE_START(swapcontext)
    /* save the preserved register values */
    stw     sp, (a0, SETJMP_SP)
    stw     l0, (a0, SETJMP_L0)
    stw     l1, (a0, SETJMP_L1)
    stw     l2, (a0, SETJMP_L2)
    stw     l3, (a0, SETJMP_L3)
    stw     l4, (a0, SETJMP_L4)
    stw     l5, (a0, SETJMP_L5)
    stw     gb, (a0, SETJMP_GB)
    stw     lr, (a0, SETJMP_LR)

    /* load the values of all the preserved registers */
    ldw     sp, (a0, SETJMP_SP)
    ldw     l0, (a0, SETJMP_L0)
    ldw     l1, (a0, SETJMP_L1)
    ldw     l2, (a0, SETJMP_L2)
    ldw     l3, (a0, SETJMP_L3)
    ldw     l4, (a0, SETJMP_L4)
    ldw     l5, (a0, SETJMP_L5)
    ldw     gb, (a0, SETJMP_GB)
    ldw     lr, (a0, SETJMP_LR)

    jmp     lr
SYM_CODE_END(swapcontext)

SYM_CODE_START(makecontext)
SYM_CODE_END(makecontext)
