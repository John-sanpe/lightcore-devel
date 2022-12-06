/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <kcoro.h>
#include <delay.h>
#include <selftest.h>

#define TEST_LOOP 100

static state kcoro_task_a(struct kcoro_work *work, void *pdata)
{
    struct kshell_context *ctx = pdata;
    unsigned int count = TEST_LOOP;

    while (count--) {
        kshell_printf(ctx, "kcoro task a running...\n");
        kcoro_yield(work);
    }

    kcoro_exit(work, -ENOERR);
    return -ENOERR;
}

static state kcoro_task_b(struct kcoro_work *work, void *pdata)
{
    struct kshell_context *ctx = pdata;
    unsigned int count = TEST_LOOP;

    while (count--) {
        kshell_printf(ctx, "kcoro task b running...\n");
        mdelay(10);
        kcoro_yield(work);
    }

    kcoro_exit(work, -ENOERR);
    return -ENOERR;
}

static state kcoro_task_c(struct kcoro_work *work, void *pdata)
{
    struct kshell_context *ctx = pdata;
    unsigned int count = TEST_LOOP;

    while (count--) {
        kshell_printf(ctx, "kcoro task c running...\n");
        kcoro_mdelay(work, 10);
        kcoro_yield(work);
    }

    kcoro_exit(work, -ENOERR);
    return -ENOERR;
}

static state kcoro_testing(struct kshell_context *ctx, void *pdata)
{
    struct kcoro_worker *worker;
    struct kcoro_work *work_a;
    struct kcoro_work *work_b;
    struct kcoro_work *work_c;
    state ret;

    worker = kcoro_worker_create();
    if (unlikely(IS_INVAL(worker)))
        return PTR_INVAL(worker);

    work_a = kcoro_work_create(worker, kcoro_task_a, ctx);
    work_b = kcoro_work_create(worker, kcoro_task_b, ctx);
    work_c = kcoro_work_create(worker, kcoro_task_c, ctx);

    if ((ret = PTR_INVAL_ZERO(work_a)) ||
        (ret = PTR_INVAL_ZERO(work_b)) ||
        (ret = PTR_INVAL_ZERO(work_c))
    ) {
        kcoro_worker_destroy(worker);
        return ret;
    }

    kcoro_dispatch(worker);
    kcoro_worker_destroy(worker);

    return -ENOERR;
}

static struct selftest_command kcoro_command = {
    .group = "kernel",
    .name = "kcoro",
    .desc = "kcoro unit test",
    .testing = kcoro_testing,
};

static state selftest_kcoro_init(void)
{
    return -ENOERR;
    return selftest_register(&kcoro_command);
}
kshell_initcall(selftest_kcoro_init);
