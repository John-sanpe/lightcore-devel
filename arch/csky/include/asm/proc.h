/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_CSKY_PROC_H_
#define _ASM_CSKY_PROC_H_

#include <types.h>
#include <state.h>
#include <asm/regs.h>
#include <asm/barrier.h>

enum clone_flags;
struct sched_task;
struct task_clone_args;

struct proc_context {

};

extern void proc_thread_setup(struct regs *regs, size_t ip, size_t sp);
extern state proc_thread_copy(struct task_clone_args *args, struct sched_task *child);
extern state proc_thread_switch(struct sched_task *prev);

extern void proc_idle(void);
extern void __noreturn proc_reset(void);
extern void __noreturn proc_halt(void);
extern void __noreturn proc_poweroff(void);

#define cpu_relax() barrier()

#endif  /* _ASM_CSKY_PROC_H_ */
