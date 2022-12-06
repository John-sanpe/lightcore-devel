/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "kcoro"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <kcoro.h>
#include <task.h>
#include <kmalloc.h>
#include <timekeeping.h>
#include <printk.h>
#include <panic.h>
#include <export.h>

static struct kcache *kcoro_work_cache;
static struct kcache *kcoro_worker_cache;

#include "delay.c"
#include "yield.c"

static inline uint64_t calc_delta(struct kcoro_work *task, uint64_t delta)
{
    uint64_t fact = 1024;
    int shift = 32;

    if (unlikely(fact >> 32)) {
        while(fact >> 32)
        {
            fact >>= 1;
            shift--;
        }
    }

    fact = (uint64_t)(uint32_t)fact * sched_prio_to_wmult[task->dyprio];

    while (fact >> 32) {
        fact >>= 1;
        shift--;
    }

    return mul_u64_u32_shr(delta, fact, shift);
}

static inline uint64_t delta_fair(struct kcoro_work *task, uint64_t delta)
{
    if (unlikely(task->dyprio != 20))
        delta = calc_delta(task, delta);
    return delta;
}

static long work_vtime_cmp(const struct rb_node *na, const struct rb_node *nb)
{
    const struct kcoro_work *work_a = container_of(na, struct kcoro_work, node);
    const struct kcoro_work *work_b = container_of(nb, struct kcoro_work, node);
    return work_a->vtime - work_b->vtime;
}

static inline struct kcoro_work *work_pick(struct kcoro_worker *worker)
{
    struct rb_node *lestmost = rb_cached_first(&worker->ready);
    return container_of_safe(lestmost, struct kcoro_work, node);
}

static inline void worker_update(struct kcoro_worker *worker)
{
    struct kcoro_work *next = work_pick(worker);

    if (next)
        worker->min_vtime = next->vtime;
    else if (worker->curr)
        worker->min_vtime = worker->curr->vtime;
    else
        worker->min_vtime = 0;
}

/**
 * worker_enqueue - enqueue work to a worker.
 * @worker: the worker pointer.
 * @work: the work to be enqueue.
 */
static inline void
worker_enqueue(struct kcoro_worker *worker, struct kcoro_work *work)
{
    rb_cached_insert(&worker->ready, &work->node, work_vtime_cmp);
    worker_update(worker);
    work->state = KCORO_READY;
}

/**
 * worker_dequeue - dequeue work form a worker.
 * @worker: the worker pointer.
 * @work: the work to be dequeue.
 */
static inline void
worker_dequeue(struct kcoro_worker *worker, struct kcoro_work *work)
{
    rb_cached_delete(&worker->ready, &work->node);
    worker_update(worker);
    work->state = KCORO_RUNNING;
}

/**
 * resource_release - release all resources of a work.
 * @work: the work to be release.
 */
static void resource_release(struct kcoro_work *work)
{
    struct kcoro_worker *worker = work->worker;

    if (work->state == KCORO_READY)
        worker_dequeue(worker, work);
    task_stack_free(work->context.stack);
    kcache_free(kcoro_work_cache, work);
}

/**
 * kcoro_work_prio - change the priority of current work.
 * @prio: the priority to change to.
 */
void kcoro_prio(struct kcoro_work *work, int prio)
{
    struct kcoro_worker *worker = work->worker;

    if (prio < -20)
        prio = 0;
    else if (prio > 19)
        prio = 39;
    else
        prio += 20;

    worker->weight -= sched_prio_to_weight[work->prio];
    worker->weight += sched_prio_to_weight[prio];
    work->prio = work->dyprio = prio;
}
EXPORT_SYMBOL(kcoro_prio);

/**
 * kcoro_yield - kcoro scheduling core.
 */
void kcoro_yield(struct kcoro_work *work)
{
    struct kcoro_worker *worker = work->worker;
    struct kcontext *context = NULL;
    struct kcoro_work *next;
    ktime_t now;

    now = timekeeping_get_time();
    work->vtime += delta_fair(work, now - work->start);

    /* Re-enqueue the previous ready work */
    if (likely(work->state == KCORO_RUNNING)) {
        if (((int64_t)(work->vtime - worker->min_vtime) > 0))
            worker_enqueue(worker, work);
        else {
            work->start = now;
            return;
        }
    } else if (work->state == KCORO_EXITED) {
        if (work->context.retval) {
            swapcontext(&work->context, &worker->retctx);
            unreachable();
        }
    }

    /* If a higher priority work is found, switch to it */
    if (likely(next = work_pick(worker))) {
        worker_dequeue(worker, next);

        next->start = now;
        if (next != work) {
            worker->curr = next;
            context = swapcontext(&work->context, &next->context);
        }
    } else {
        context = swapcontext(&work->context, &worker->idle.context);
    }

    barrier();

    /* If we switch from a context, then we get the previous work */
    if (context) {
        work = container_of(context, struct kcoro_work, context);

        /* Exit the previous work by circumstances */
        if (unlikely(work->state == KCORO_EXITED)) {
            /**
             * If it has been switched to other tasks, then we
             * can directly release this work.
             */
            resource_release(work);
        }
    }
}
EXPORT_SYMBOL(kcoro_yield);

state kcoro_dispatch(struct kcoro_worker *worker)
{
    struct kcoro_work *curr, *next;
    struct kcontext *context;
    state retval;

    do {
        next = work_pick(worker);
        if (next) {
            worker_dequeue(worker, next);
            worker->curr = next;

            /* Always retctx running here so don't need memory barriers */
            context = swapcontext(&worker->retctx, &next->context);
            curr = container_of(context, struct kcoro_work, context);

            /* Exit from the end of a work */
            retval = curr->context.retval;
            resource_release(curr);
        }
    } while (next && !retval);

    return retval;
}
EXPORT_SYMBOL(kcoro_dispatch);

void kcoro_exit(struct kcoro_work *work, state retval)
{
    work->context.retval = retval;
    work->state = KCORO_EXITED;
    kcoro_yield(work);
}
EXPORT_SYMBOL(kcoro_exit);

struct kcoro_work *
kcoro_work_create(struct kcoro_worker *worker, kcoro_entry_t entry, void *pdata)
{
    struct kcoro_work *work;
    void *stack;

    work = kcache_zalloc(kcoro_work_cache, GFP_KERNEL);
    if (!work)
        return NULL;

    stack = task_stack_alloc();
    if (!stack) {
        kcache_free(kcoro_work_cache, work);
        return NULL;
    }

    work->prio = 20;
    work->dyprio = 20;
    work->start = timekeeping_get_time();
    work->vtime = worker->min_vtime;
    work->worker = worker;

    work->context.stack = stack;
    work->context.ssize = THREAD_SIZE;
    work->context.link = &worker->retctx;

    makecontext(&work->context, (state (*)(void))entry, 2, work, pdata);
    worker_enqueue(worker, work);

    return work;
}
EXPORT_SYMBOL(kcoro_work_create);

void kcoro_work_destroy(struct kcoro_work *work)
{
    // struct kcoro_worker *worker = work->worker;


    // if (work->state != KCORO_READY)
    //     work->state = KCORO_EXITED;
    // else {
    //     resource_release(work);
    //     return;
    // }

}
EXPORT_SYMBOL(kcoro_work_destroy);

struct kcoro_worker *kcoro_worker_create()
{
    struct kcoro_worker *worker;
    void *stack;

    worker = kcache_zalloc(kcoro_worker_cache, GFP_KERNEL);
    if (unlikely(!worker))
        return NULL;

    stack = task_stack_alloc();
    if (unlikely(!stack)) {
        kcache_free(kcoro_worker_cache, worker);
        return NULL;
    }

    worker->idle.worker = worker;
    worker->idle.context.stack = stack;
    worker->idle.context.ssize = THREAD_SIZE;
    worker->idle.context.link = &worker->retctx;
    makecontext(&worker->idle.context, (state (*)(void))idle_task, 1, &worker->idle);

    return worker;
}
EXPORT_SYMBOL(kcoro_worker_create);

void kcoro_worker_destroy(struct kcoro_worker *worker)
{
    kfree(worker);
}
EXPORT_SYMBOL(kcoro_worker_destroy);

void __init kcoro_init(void)
{
    kcoro_worker_cache = kcache_create(
        "kcoro-worker", sizeof(struct kcoro_worker),
        KCACHE_PANIC
    );
    kcoro_work_cache = kcache_create(
        "kcoro-work", sizeof(struct kcoro_work),
        KCACHE_PANIC
    );
}

