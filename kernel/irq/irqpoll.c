/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <irqpoll.h>
#include <ticktime.h>
#include <overflow.h>
#include <percpu.h>
#include <softirq.h>
#include <initcall.h>
#include <panic.h>
#include <export.h>

static unsigned int __read_mostly irqpoll_burst = 256;
static DEFINE_PERCPU(struct list_head, irqpoll_list);

static void irqpoll_softirq_handle(void *pdata);
static DEFINE_SOFTIRQ(irqpoll_softirq, irqpoll_softirq_handle, NULL, 0);

static void irqpoll_softirq_handle(void *pdata)
{
    struct list_head *head = thiscpu_ptr(&irqpoll_list);
    unsigned int weight, works, burst = irqpoll_burst;
    ttime_t start_time = ticktime;
    struct irqpoll *poll;

    while (!list_check_empty(head)) {
        if (!burst || ttime_after(ticktime, start_time)) {
            return;
        }

        irq_local_enable();

        poll = list_first_entry(head, struct irqpoll, list);
        weight = poll->weight;
        works = 0;

        if (irqpoll_test_queued(poll))
            works = poll->entry(poll, weight);

        burst = overflow_sub(burst, works);
        irq_local_disable();

		if (works >= weight) {
			if (irqpoll_test_disable(poll))
				__irq_poll_complete(iop);
			else
				list_move_prev(head, &poll->list);
		}
    }
}

void irqpoll_enable(struct irqpoll *poll)
{
	BUG_ON(!irqpoll_atomic_test_queued(poll));

}
EXPORT_SYMBOL(irqpoll_enable);

void irqpoll_disable(struct irqpoll *poll)
{
    irqpoll_atomic_set_disable(poll);
    while (irqpoll_atomic_test_set_queued(poll))
        msleep(1);
    irqpoll_atomic_clr_disable(poll);
}
EXPORT_SYMBOL(irqpoll_disable);

void __init irqpoll_init(void)
{
    softirq_register();
}
