/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_GENERIC_BUILTIN_FLS_H_
#define _ASM_GENERIC_BUILTIN_FLS_H_

#include <bits.h>

#define flsuf flsuf
static __always_inline unsigned int flsuf(unsigned long value)
{
	return BITS_PER_LONG - 1 - __builtin_clzl(value);
}

#endif  /* _ASM_GENERIC_BUILTIN_FLS_H_ */
