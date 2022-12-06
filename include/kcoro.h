/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _KCORO_H_
#define _KCORO_H_

#include <list.h>
#include <rbtree.h>
#include <kcontext.h>

/**
 * enum kcoro_state -
 * @ready:
 * @running:
 * @blocked:
 * @exited:
 */
enum kcoro_state {
    KCORO_READY     = 0,
    KCORO_RUNNING   = 1,
    KCORO_BLOCKED   = 2,
    KCORO_EXITED    = 3,
};

/**
 * kcoro_work - describe a kcoroutine work.
 * @node: node add to worker.
 * @worker: bound worker.
 * @name: kcoro work name.
 * @context: the work context.
 * @state: running state.
 * @start: work start time.
 * @vtime: relative running time.
 * @prio: static priority.
 * @dyprio: dynamic priority.
 * @pdata: work entry pdata.
 */
struct kcoro_work {
    struct rb_node node;
    struct kcoro_worker *worker;

    struct kcontext context;
    enum kcoro_state state;

    ktime_t start;
    uint64_t vtime;
    ttime_t timeout;

    int prio;
    int dyprio;
};

/**
 * kcoro_worker - describe a kcoroutine worker.
 * @lock: used to lock the red black tree and task status.
 * @list: list node for system management.
 * @task: point to the parent task.
 * @ready: ready queue ont this worker.
 * @retctx: context use for task return.
 * @curr: current running kcoro work.
 * @min_vtime: minimum virtual time in this worker.
 * @weight: worker weight.
 */
struct kcoro_worker {
    struct rb_root_cached ready;
    struct rb_root_cached blocked;

    struct list_head list;
    struct kcoro_work *curr;
    struct kcoro_work idle;

    struct kcontext retctx;
    uint64_t min_vtime;
    uint64_t weight;
};

typedef state (*kcoro_entry_t)(struct kcoro_work *work, void *pdata);

static inline void kcoro_dyprio_inc(struct kcoro_work *work)
{
    if (work->dyprio < 39)
        work->dyprio++;
}

static inline void kcoro_dyprio_dec(struct kcoro_work *work)
{
    if (work->dyprio > 0)
        work->dyprio--;
}

static inline void kcoro_dyprio_reset(struct kcoro_work *work)
{
    if (work->dyprio != work->prio)
        work->dyprio = work->prio;
}

/* Kcoro sleep */
extern void kcoro_ndelay(struct kcoro_work *work, unsigned int nsec);
extern void kcoro_udelay(struct kcoro_work *work, unsigned int usec);
extern void kcoro_mdelay(struct kcoro_work *work, unsigned int msec);
extern void kcoro_msleep(struct kcoro_work *work, unsigned int msec);

/* Kcoro dispatch */
extern void kcoro_prio(struct kcoro_work *work, int prio);
extern void kcoro_yield(struct kcoro_work *work);
extern void kcoro_exit(struct kcoro_work *work, state retval);
extern state kcoro_dispatch(struct kcoro_worker *worker);

/* Kcoro worker work */
extern struct kcoro_work *kcoro_work_create(struct kcoro_worker *worker, kcoro_entry_t entry, void *pdata);
extern void kcoro_work_destroy(struct kcoro_work *work);
extern struct kcoro_worker *kcoro_worker_create();
extern void kcoro_worker_destroy(struct kcoro_worker *worker);
extern void __init kcoro_init(void);

#endif  /* _KCORO_H_ */
