/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _KBOOT_H_
#define _KBOOT_H_

#include <memory.h>
#include <lib.h>

#define kernel_entry (pa_to_va(NORMAL_OFFSET))

extern uint8_t _ld_piggy_start;
extern uint8_t _ld_piggy_end;
extern uint8_t _ld_heap_start;
extern uint8_t _ld_heap_end;

#define piggy_start ((void *)&_ld_piggy_start)
#define piggy_end   ((void *)&_ld_piggy_end)
#define piggy_size  ((size_t)piggy_end)

#define heap_start  ((void *)&_ld_heap_start)
#define heap_end    ((void *)&_ld_heap_end)
#define heap_size   ((size_t)heap_end)

extern void __noreturn kernel_start(void *addr);
extern void __noreturn halt(void);
extern void kernel_map(void);

#endif  /* _KBOOT_H_ */
