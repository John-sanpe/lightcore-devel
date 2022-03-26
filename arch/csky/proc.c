/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kernel.h>
#include <task.h>
#include <proc.h>
#include <kcontext.h>
#include <irqflags.h>

state proc_thread_copy(struct task_clone_args *args, struct sched_task *child)
{
    // if (child->flags & SCHED_TASK_KTHREAD) {
    //     child->kcontext.stack = child->stack;
    //     child->kcontext.ssize = THREAD_SIZE;
    //     makecontext(&child->kcontext, (state (*)(void))entry_kthread_return, 2, args->entry, args->arg);
    //     return -ENOERR;
    // }

    return -ENOERR;
}

state proc_thread_switch(struct sched_task *prev)
{
    return -ENOERR;
}

void proc_idle(void)
{
#if defined(CONFIG_CPU_PM_WAIT)
    asm volatile ("wait");
#elif defined(CONFIG_CPU_PM_DOZE)
    asm volatile ("doze");
#elif defined(CONFIG_CPU_PM_STOP)
    asm volatile ("stop");
#endif
}

void __noreturn proc_halt(void)
{
    irq_local_disable();
    for (;;)
    proc_idle();
}

void __noreturn proc_poweroff(void)
{
    proc_halt();
}

void __noreturn proc_reset(void)
{
    proc_halt();
}
