/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <types.h>
#include <export.h>
#include <delay.h>

void arch_proc_do_loop(unsigned long loops)
{
    asm volatile (
		"mov    r0, r0  \n"
		"1:     declt %0\n"
		"bf	    1b"
		: "=r"(loops)
		: "0"(loops)
    );
}

void arch_proc_const_delay(unsigned long loops)
{
    arch_proc_do_loop(((uint64_t)loops * loops_per_tick * CONFIG_SYSTICK_FREQ) >> 32);
}

void arch_proc_ndelay(unsigned long nsecs)
{
    arch_proc_const_delay(nsecs * 5);
}

void arch_proc_udelay(unsigned long usecs)
{
    arch_proc_const_delay(usecs * 4295);
}

EXPORT_SYMBOL(arch_proc_do_loop);
EXPORT_SYMBOL(arch_proc_const_delay);
EXPORT_SYMBOL(arch_proc_ndelay);
EXPORT_SYMBOL(arch_proc_udelay);
