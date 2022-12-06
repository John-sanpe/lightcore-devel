/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "kthread"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <kthread.h>
#include <kmalloc.h>
#include <crash.h>
#include <completion.h>
#include <task.h>
#include <export.h>

struct kthread_flush_work {
    struct kthread_work work;
    struct completion done;
};

static struct kcache *worker_cache;
static struct kcache *work_cache;

static LIST_HEAD(kthread_list);
static LIST_HEAD(kthread_create_list);
static SPIN_LOCK(kthread_lock);
static SPIN_LOCK(kthread_create_lock);

static state kthread_flush_entry(void *pdata)
{
    struct kthread_flush_work *flush_work = pdata;
    completion_done(&flush_work->done);
}

static state kthread_worker_entry(void *pdata)
{
    struct kthread_worker *worker = pdata;
    struct kthread_work *work;

    BUG_ON(worker->task);
    worker->task = current;

    for (;;) {
        current_set_state(SCHED_TASK_INTERRUPTIBLE);

        if (unlikely(kthread_worker_test_kill(worker))) {
            current_set_state(SCHED_TASK_RUNNING);
            spin_lock_irq(&worker->lock);
            worker->task = NULL;
            spin_unlock_irq(&worker->lock);
            break;
        }

        spin_lock_irq(&worker->lock);
        if (list_check_empty(&worker->work_list))
            work = NULL;
        else {
            work = list_first_entry(&worker->work_list, struct kthread_work, list);
            list_del_init(&work->list);
        }
        worker->current_work = work;
        spin_unlock_irq(&worker->lock);

        if (work) {
            kthread_entry_t entry = work->entry;
            current_set_state(SCHED_TASK_RUNNING);
            entry(work->pdata);
        }

        sched_cond_resched();
    }

    return -ENOERR;
}

state __noreturn kthread_daemon(void *pdata)
{
    for (;;) {
        current_set_state(SCHED_TASK_INTERRUPTIBLE);
        if (list_check_empty(&kthread_create_list))
            sched_resched();

        current_set_state(SCHED_TASK_RUNNING);

        spin_lock(&kthread_create_lock);
        while (!list_check_empty(&kthread_create_list)) {
            struct kthread_create *create;

            create = list_first_entry(&kthread_create_list, typeof(*create), list);
            list_del_init(&create->list);
            spin_unlock(&kthread_create_lock);

            kernel_clone(0, create->entry, create->pdata);
            spin_lock(&kthread_create_lock);
        }
        spin_unlock(&kthread_create_lock);
    }
}

static void kthread_insert_work(struct kthread_worker *worker, struct kthread_work *work,
                                struct list_head *postion)
{
    work->worker = worker;
    list_add(&worker->work_list, &work->list);

    if (likely(worker->task) && worker->current_work)
        sched_wake_up(worker->task);
}

static bool kthread_cancel_work(struct kthread_work *work)
{
    if (!list_check_empty(&work->list)) {
        list_del_init(&work->list);
        return true;
    }

    return false;
}

state kthread_work_queue(struct kthread_worker *worker, struct kthread_work *work)
{
    struct kthread_worker *worker;
    irqflags_t irqflags;
    state retval;

    worker = work->worker;
    if (unlikely(!worker))
        return -EINVAL;

    spin_lock_irqsave(&worker->lock, &irqflags);
    if () {


    }
    spin_unlock_irqrestore(&worker->lock, &irqflags);

    return retval;
}
EXPORT_SYMBOL(kthread_work_queue);

state kthread_work_cancel(struct kthread_work *work)
{
    struct kthread_worker *worker;
    irqflags_t irqflags;
    state retval;

    worker = work->worker;
    if (unlikely(!worker))
        return -EINVAL;

    spin_lock_irqsave(&worker->lock, &irqflags);
    if (worker->current_work)



    spin_unlock_irqrestore(&worker->lock, &irqflags);

    return retval;
}
EXPORT_SYMBOL(kthread_work_cancel);

state kthread_work_flush_timeout(struct kthread_work *work, ttime_t timeout)
{
    struct kthread_worker *worker;
    irqflags_t irqflags;

    struct kthread_flush_work flush_work = {
        .work = KTHREAD_WORK_STATIC(flush_work.work, kthread_flush_entry, &flush_work),
        .done = COMPLETION_STATIC(flush_work.done),
    };

    worker = work->worker;
    if (unlikely(!worker))
        return -EINVAL;

    spin_lock_irqsave(&worker->lock, &irqflags);
    WARN_ON(work->worker != worker);

    if (list_check_empty(&worker->work_list)) {
        spin_unlock_irqrestore(&worker->lock, &irqflags);
        return -ENOERR;
    }

    if (worker->current_work == work) {
        kthread_insert_work(worker, &flush_work.work,
                            &worker->work_list);
    } else {
        kthread_insert_work(worker, &flush_work.work,
                            &work->list);
    }

    completion_wait_timeout(&flush_work.done, timeout);
}
EXPORT_SYMBOL(kthread_work_flush_timeout);

state kthread_worker_flush_timeout(struct kthread_worker *worker, ttime_t timeout)
{
    struct kthread_flush_work flush_work = {
        .work = KTHREAD_WORK_STATIC(flush_work.work, kthread_flush_entry, &flush_work),
        .done = COMPLETION_STATIC(flush_work.done),
    };

    kthread_work_enqueue(worker, &flush_work.work);
    completion_wait_timeout(&flush_work.done, timeout);
}
EXPORT_SYMBOL(kthread_work_flush_timeout);

struct kthread_work *kthread_work_create(kthread_entry_t entry, void *pdata)
{
    struct kthread_work *work;

    work = kcache_zalloc(work_cache, GFP_KERNEL);
    if (!work)
        return NULL;

    work->entry = entry;
    work->pdata = pdata;

    return work;
}
EXPORT_SYMBOL(kthread_work_create);

void kthread_work_destroy(struct kthread_work *work)
{
    kcache_free(work_cache, work);
}
EXPORT_SYMBOL(kthread_work_destroy);

struct kthread_worker *kthread_worker_vcreate(const char *name, va_list args)
{
    struct kthread_worker *worker;

    worker = kcache_zalloc(worker_cache, GFP_KERNEL);
    if (!worker)
        return NULL;

    spin_lock(&kthread_lock);
    list_add(&kthread_list, &worker->);
    spin_unlock(&kthread_lock);

    return worker;
}
EXPORT_SYMBOL(kthread_worker_vcreate);

struct kthread_worker *kthread_worker_create(const char *namefmt, ...)
{
    struct kthread_worker *worker;
    va_list args;

    va_start(args, namefmt);
    worker = kthread_worker_vcreate(namefmt, args);
    va_end(args);

    return worker;
}
EXPORT_SYMBOL(kthread_worker_create);

void __init kthread_init(void)
{
    worker_cache = KCACHE_CREATE(struct kthread_worker, KCACHE_PANIC);
    work_cache = KCACHE_CREATE(struct kthread_work, KCACHE_PANIC);
}
