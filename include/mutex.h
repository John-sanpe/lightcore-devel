/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _MUTEX_H_
#define _MUTEX_H_

#include <types.h>
#include <spinlock.h>
#include <list.h>

struct mutex {
    spinlock_t lock;
    atomic_t atomic;
    struct list_head list;
};

#define MUTEX_LOCK(name) \
    struct mutex name = {}

extern void mutex_lock(struct mutex *mutex);
extern bool mutex_trylock(struct mutex *mutex);
extern void mutex_unlock(struct mutex *mutex);

#endif
