/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <fmath.h>
#include <export.h>

static double __fmin(double x, double y)
{
	if (isnan(x))
		return y;
	if (isnan(y))
		return x;
	/* handle signed zeros, see C99 Annex F.9.9.2 */
	if (signbit(x) != signbit(y))
		return signbit(x) ? x : y;
	return x < y ? x : y;
}

extern __typeof(__fmin) fmin __attribute__((weak, alias("__fmin")));
EXPORT_SYMBOL(fmin);
