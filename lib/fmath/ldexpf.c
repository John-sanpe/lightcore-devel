/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <fmath.h>
#include <export.h>

float ldexpf(float x, int n)
{
	return scalbnf(x, n);
}
EXPORT_SYMBOL(ldexpf);
