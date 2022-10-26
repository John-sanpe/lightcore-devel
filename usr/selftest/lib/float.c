/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <selftest.h>

static state float_testing(struct kshell_context *ctx, void *pdata)
{
    volatile double a, b, c, d, e, f, g;

    kshell_printf(ctx, "test float operation: ");
    a = 4.0;
    b = 1e-15;
    c = 1e-310;

    /* Sets precision flag */
    d = a + b;

    /* Result depends on rounding mode */
    e = a + b / 2;

    /* Denormal and very large values */
    f = b / c;

    /* Depends on denormal support */
    g = a + c * f;

    if (d <= a || e <= a || g <= a) {
        kshell_printf(ctx, "failed\n");
        return -EINVAL;
    }

    kshell_printf(ctx, "passed\n");
    return -ENOERR;
}

static struct selftest_command glob_test_command = {
    .group = "lib",
    .name = "float",
    .desc = "float operation test",
    .testing = float_testing,
};

static state selftest_glob_init(void)
{
    return selftest_register(&glob_test_command);
}
kshell_initcall(selftest_glob_init);
