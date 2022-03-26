/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_CSKY_SETJMP_H_
#define _ASM_CSKY_SETJMP_H_

#define JMP_BUF_SP      0
#define JMP_BUF_L0      1
#define JMP_BUF_L1      2
#define JMP_BUF_L2      3
#define JMP_BUF_L3      4
#define JMP_BUF_L4      5
#define JMP_BUF_L5      6
#define JMP_BUF_GB      7
#define JMP_BUF_LR      8
#define JMP_BUF_REGS    9

#include <asm-generic/setjmp.h>

#endif /* _ASM_CSKY_SETJMP_H_ */
