/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <fmath.h>
#include <export.h>

static double __fabs(double x)
{
	union {double f; uint64_t i;} u = {x};
	u.i &= -1ULL/2;
	return u.f;
}

extern __typeof(__fabs) fabs __attribute__((weak, alias("__fabs")));
EXPORT_SYMBOL(fabs);
