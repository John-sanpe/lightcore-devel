/* SPDX-License-Identifier: GPL-2.0-or-later */
#include "soft-fp.h"
#include "double.h"

#include <libgcc.h>
#include <export.h>

DFtype __weak __adddf3(DFtype a, DFtype b)
{
    FP_DECL_EX;
    FP_DECL_D(A);
    FP_DECL_D(B);
    FP_DECL_D(R);
    DFtype r;

    FP_INIT_ROUNDMODE;
    FP_UNPACK_SEMIRAW_D(A, a);
    FP_UNPACK_SEMIRAW_D(B, b);
    FP_ADD_D(R, A, B);
    FP_PACK_SEMIRAW_D(r, R);
    FP_HANDLE_EXCEPTIONS;

    return r;
}
EXPORT_SYMBOL(__adddf3);
