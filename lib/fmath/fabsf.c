/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <fmath.h>
#include <export.h>

static float __fabsf(float x)
{
	union {float f; uint32_t i;} u = {x};
	u.i &= 0x7fffffff;
	return u.f;
}

extern __typeof(__fabsf) fabsf __attribute__((weak, alias("__fabsf")));
EXPORT_SYMBOL(fabsf);
