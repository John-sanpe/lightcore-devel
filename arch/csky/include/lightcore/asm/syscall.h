/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _LIGHTCORE_ASM_CSKY_SYSCALL_H_
#define _LIGHTCORE_ASM_CSKY_SYSCALL_H_

static __always_inline long
syscall(unsigned int callnr, long arg1, long arg2,
        long arg3, long arg4, long arg5, long arg6)
{
    long retval;

    asm volatile (
        ""
    );

    return retval;
}

#endif  /* _LIGHTCORE_ASM_CSKY_SYSCALL_H_ */
