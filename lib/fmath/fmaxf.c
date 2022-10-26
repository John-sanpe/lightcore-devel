/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <fmath.h>
#include <export.h>

float __weak fmaxf(float x, float y)
{
	if (isnan(x))
		return y;
	if (isnan(y))
		return x;
	/* handle signed zeroes, see C99 Annex F.9.9.2 */
	if (signbit(x) != signbit(y))
		return signbit(x) ? y : x;
	return x < y ? y : x;
}
EXPORT_SYMBOL(fmaxf);
