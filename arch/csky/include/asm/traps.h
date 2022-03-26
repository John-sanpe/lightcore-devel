/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_CSKY_TRAPS_H_
#define _ASM_CSKY_TRAPS_H_

#include <asm/regs.h>

static inline bool trap_user_mode(struct regs *regs)
{
    return !(regs->psr & PSR_S);
}

#endif /* _ASM_CSKY_TRAPS_H_ */