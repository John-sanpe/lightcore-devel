/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _KTHREAD_H_
#define _KTHREAD_H_

#include <list.h>
#include <ticktime.h>
#include <bitflags.h>

struct sched_task;
typedef state (*kthread_entry_t)(void *pdata);

enum kthread_worker_flags {
    __KTHREAD_WORKER_KILL = 0,
    __KTHREAD_WORKER_PARK = 1,
};

#define KTHREAD_WORKER_KILL     BIT(__KTHREAD_WORKER_KILL)
#define KTHREAD_WORKER_PARK     BIT(__KTHREAD_WORKER_PARK)

struct kthread_work {
    struct list_head list;
    struct kthread_worker *worker;
    unsigned long flags;

    kthread_entry_t entry;
    void *pdata;
};

struct kthread_worker {
    spinlock_t lock;
    struct sched_task *task;
    unsigned long flags;

    struct list_head work_list;
    struct kthread_work *current_work;
};

struct kthread_create {
    struct list_head list;
    state (*entry)(void *pdata);
    void *pdata;
};

#define KTHREAD_WORK_STATIC(_work, _entry, _pdata) {    \
	.list = LIST_HEAD_STATIC((_work).list),             \
	.entry = (_entry), .pdata = (_pdata),               \
}

#define KTHREAD_WORK_INIT(work, entry, pdata) \
    (struct kthread_work)KTHREAD_WORK_STATIC(work, entry, pdata)

#define DEFINE_KTHREAD_WORK(work, entry, pdata) \
    struct kthread_work work = KTHREAD_WORK_INIT(work, entry, pdata)

GENERIC_STRUCT_BITOPS(kthread_worker, struct kthread_worker, flags)
GENERIC_STRUCT_FLAG(kthread_worker, struct kthread_worker, flags, kill, __KTHREAD_WORKER_KILL)
GENERIC_STRUCT_FLAG(kthread_worker, struct kthread_worker, flags, park, __KTHREAD_WORKER_PARK)

static inline void kthread_work_init(struct kthread_work *work, kthread_entry_t entry, void *pdata)
{
    *work = KTHREAD_WORK_INIT(*work, entry, pdata);
}

extern state kthread_work_queue(struct kthread_worker *worker, struct kthread_work *work);
extern state kthread_work_cancel(struct kthread_work *work);
extern state kthread_work_flush_timeout(struct kthread_work *work, ttime_t timeout);
extern state kthread_worker_flush_timeout(struct kthread_worker *worker, ttime_t timeout);

static inline state kthread_work_flush(struct kthread_work *work)
{
    return kthread_work_flush_timeout(work, TTIME_MAX);
}

static inline state kthread_worker_flush(struct kthread_worker *worker)
{
    return kthread_worker_flush_timeout(worker, TTIME_MAX);
}

extern struct kthread_work *kthread_work_create(kthread_entry_t entry, void *pdata);
extern void kthread_work_destroy(struct kthread_work *work);
extern struct kthread_worker *kthread_worker_vcreate(const char *namefmt, va_list args);
extern struct kthread_worker *kthread_worker_create(const char *namefmt, ...);
extern void kthread_worker_destroy(struct kthread_worker *worker);

extern state kthread_daemon(void *pdata);
extern void __init kthread_init(void);

#endif  /* _KTHREAD_H_ */
