/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <fmath.h>
#include <export.h>

double fdim(double x, double y)
{
	if (isnan(x))
		return x;
	if (isnan(y))
		return y;
	return x > y ? x - y : 0;
}
EXPORT_SYMBOL(fdim);
