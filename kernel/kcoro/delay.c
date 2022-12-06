/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

void kcoro_ndelay(struct kcoro_work *work, unsigned int nsec)
{
    ktime_t timeout = ktime_add_ns(timekeeping_get_time(), nsec);
    while (ktime_before(timekeeping_get_time(), timeout)) {
        kcoro_dyprio_inc(work);
        kcoro_yield(work);
    }
    kcoro_dyprio_reset(work);
}
EXPORT_SYMBOL(kcoro_ndelay);

void kcoro_udelay(struct kcoro_work *work, unsigned int usec)
{
    ktime_t timeout = ktime_add_us(timekeeping_get_time(), usec);
    while (ktime_before(timekeeping_get_time(), timeout)) {
        kcoro_dyprio_inc(work);
        kcoro_yield(work);
    }
    kcoro_dyprio_reset(work);
}
EXPORT_SYMBOL(kcoro_udelay);

void kcoro_mdelay(struct kcoro_work *work, unsigned int msec)
{
    ktime_t timeout = ktime_add_ms(timekeeping_get_time(), msec);
    while (ktime_before(timekeeping_get_time(), timeout)) {
        kcoro_dyprio_inc(work);
        kcoro_yield(work);
    }
    kcoro_dyprio_reset(work);
}
EXPORT_SYMBOL(kcoro_mdelay);
