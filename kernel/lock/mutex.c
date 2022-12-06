/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <mutex.h>
#include <sched.h>
#include <export.h>

struct mutex_waiter {
    struct list_head list;
    struct sched_task *task;
};

#define MUTEX_FLAG_WAITERS  BIT(0)
#define MUTEX_FLAGS         BIT_RANGE(1, 0)

static inline void mutex_flag_set(struct mutex *mutex, atomic_t flags)
{
    atomic_or(&mutex->owner, flags);
}

static inline void mutex_flag_clear(struct mutex *mutex, atomic_t flags)
{
    atomic_and(&mutex->owner, ~flags);
}

static inline bool mutex_fast_lock(struct mutex *mutex)
{
    unsigned long curr = (uintptr_t)current;
    unsigned long zero = 0;
    return atomic_try_cmpxchg(&mutex->owner, &zero, curr);
}

static inline bool mutex_fast_unlock(struct mutex *mutex)
{
    unsigned long curr = (uintptr_t)current;
    return atomic_try_cmpxchg(&mutex->owner, &curr, 0UL);
}

static inline struct sched_task *mutex_common_trylock(struct mutex *mutex)
{
    unsigned long curr = (uintptr_t)current;
    unsigned long owner;

    owner = atomic_read(&mutex->owner);
    for (;;) {
        unsigned long task, flags;

        task = owner & ~MUTEX_FLAGS;
        flags = owner & MUTEX_FLAGS;

        if (!task)
            task = curr;
        else {

        }



    }

}

static inline void mutex_handoff(struct mutex *mutex, struct sched_task *task)
{
    unsigned long owner, new;

    owner = atomic_read(&mutex->owner);
    for (;;) {




    }
}

static __sched void mutex_slowpath_lock(struct mutex *mutex)
{

}

static __sched void mutex_slowpath_unlock(struct mutex *mutex)
{
    struct sched_task *wtask = NULL;
    DEFINE_WAKEQUEUE(queue);
    unsigned long owner;

    owner = atomic_read(&mutex->owner);
    for (;;) {
		if (owner & MUTEX_FLAG_HANDOFF)
			break;

        if (atomic_try_cmpxchg(&mutex->owner, &owner, __owner_flags(owner))) {
            if (owner & MUTEX_FLAG_WAITERS)
                break;

            return;
        }
    }

    spin_lock(&mutex->lock);
    if (!list_check_empty(&mutex->waits)) {
        struct mutex_waiter *waiter;

        waiter = list_first_entry(&mutex->waits, struct mutex_waiter, list);
        wtask = waiter->task;
        wakequeue_add(&queue, wtask);
    }

    spin_unlock(&mutex->lock);
    wakequeue_action(&queue);
}

bool __sched mutex_trylock(struct mutex *mutex)
{
    return true;
}
EXPORT_SYMBOL(mutex_trylock);

void __sched mutex_lock(struct mutex *mutex)
{
    if (likely(mutex_fast_lock(mutex)))
        return;
    mutex_slowpath_lock(mutex);
}
EXPORT_SYMBOL(mutex_lock);

void __sched mutex_unlock(struct mutex *mutex)
{
    if (likely(mutex_fast_unlock(mutex)))
        return;
    mutex_slowpath_unlock(mutex);
}
EXPORT_SYMBOL(mutex_unlock);
