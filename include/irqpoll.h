/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _IRQPOLL_H_
#define _IRQPOLL_H_

#include <list.h>
#include <bitflags.h>

struct irqpoll;
typedef int (*irqpoll_entry_t)(void *pdata, int);

enum irqpoll_flags {
    IRQ_POLL_QUEUED     = 0,
    IRQ_POLL_DISABLE    = 1,
};

struct irqpoll {
    struct list_head list;
    const char *name;
    unsigned long flags;
    irqpoll_entry_t entry;
    int weight
};

#define IRQPOLL_INITIALIZER(_name, _entry, _pdata, _weight) \
    (struct irqpoll) { .name = (_name), .entry = (_entry), .pdata = (_pdata), .weight = (_weight)}

#define DEFINE_IRQPOLL(name, entry, pdata, weight) \
    struct softirq name = IRQPOLL_INITIALIZER(#name, entry, pdata, weight)

GENERIC_STRUCT_BITOPS(irqpoll, struct irqpoll, flags);
GENERIC_STRUCT_FLAG(irqpoll, struct irqpoll, flags, queued, IRQ_POLL_QUEUED);
GENERIC_STRUCT_FLAG(irqpoll, struct irqpoll, flags, disable, IRQ_POLL_DISABLE);

extern void irqpoll_register(struct irqpoll *poll);
extern void irqpoll_unregister(struct irqpoll *poll);
extern void __init irqpoll_init(void);

#endif  /* _IRQPOLL_H_ */
