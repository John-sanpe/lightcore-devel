/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <fmath.h>
#include <export.h>

double ldexp(double x, int n)
{
	return scalbn(x, n);
}
EXPORT_SYMBOL(ldexp);
