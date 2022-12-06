/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _KTHREAD_H_
#define _KTHREAD_H_

#ifndef __HOSTPROG__
# include <types.h>
#endif

#ifdef CONFIG_ARCH_32BIT
typedef uint32_t ksymbol_size;
#else
typedef uint64_t ksymbol_size;
#endif

struct ksymbol_enrty {


};

#endif  /* __HOSTPROG__ */
