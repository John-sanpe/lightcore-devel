/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <fmath.h>
#include <export.h>

double scalbln(double x, long n)
{
	if (n > INT_MAX)
		n = INT_MAX;
	else if (n < INT_MIN)
		n = INT_MIN;
	return scalbn(x, n);
}
EXPORT_SYMBOL(scalbln);
