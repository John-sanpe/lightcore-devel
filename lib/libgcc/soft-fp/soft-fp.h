/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _LOCAL_SOFT_FP_H_
#define _LOCAL_SOFT_FP_H_

#include <asm/sfp-machine.h>

#define _FP_ZERO_INIT     = 0
#define _FP_WORKBITS      3
#define _FP_WORK_LSB      ((_FP_W_TYPE) 1 << 3)
#define _FP_WORK_ROUND    ((_FP_W_TYPE) 1 << 2)
#define _FP_WORK_GUARD    ((_FP_W_TYPE) 1 << 1)
#define _FP_WORK_STICKY   ((_FP_W_TYPE) 1 << 0)

#ifndef FP_RND_NEAREST
# define FP_RND_NEAREST		0
# define FP_RND_ZERO		1
# define FP_RND_PINF		2
# define FP_RND_MINF		3
#endif
#ifndef FP_ROUNDMODE
# define FP_ROUNDMODE		FP_RND_NEAREST
#endif

#ifndef FP_EX_INVALID
# define FP_EX_INVALID		0
#endif
#ifndef FP_EX_OVERFLOW
# define FP_EX_OVERFLOW		0
#endif
#ifndef FP_EX_UNDERFLOW
# define FP_EX_UNDERFLOW	0
#endif
#ifndef FP_EX_DIVZERO
# define FP_EX_DIVZERO		0
#endif
#ifndef FP_EX_INEXACT
# define FP_EX_INEXACT		0
#endif
#ifndef FP_EX_DENORM
# define FP_EX_DENORM		0
#endif
#ifndef FP_EX_INVALID_SNAN
# define FP_EX_INVALID_SNAN	0
#endif
#ifndef FP_EX_INVALID_IMZ
# define FP_EX_INVALID_IMZ	0
#endif
#ifndef FP_EX_INVALID_IMZ_FMA
# define FP_EX_INVALID_IMZ_FMA	0
#endif
#ifndef FP_EX_INVALID_ISI
# define FP_EX_INVALID_ISI	0
#endif
#ifndef FP_EX_INVALID_ZDZ
# define FP_EX_INVALID_ZDZ	0
#endif
#ifndef FP_EX_INVALID_IDI
# define FP_EX_INVALID_IDI	0
#endif
#ifndef FP_EX_INVALID_SQRT
# define FP_EX_INVALID_SQRT	0
#endif
#ifndef FP_EX_INVALID_CVI
# define FP_EX_INVALID_CVI	0
#endif
#ifndef FP_EX_INVALID_VC
# define FP_EX_INVALID_VC	0
#endif
#ifndef _FP_STRUCT_LAYOUT
# define _FP_STRUCT_LAYOUT
#endif

#ifdef _FP_DECL_EX
# define FP_DECL_EX					\
  int _fex = 0;						\
  _FP_DECL_EX
#else
# define FP_DECL_EX int _fex = 0
#endif

#ifndef FP_INIT_ROUNDMODE
# define FP_INIT_ROUNDMODE do {} while (0)
#endif
#ifndef FP_INIT_TRAPPING_EXCEPTIONS
# define FP_INIT_TRAPPING_EXCEPTIONS FP_INIT_ROUNDMODE
#endif
#ifndef FP_INIT_EXCEPTIONS
# define FP_INIT_EXCEPTIONS FP_INIT_TRAPPING_EXCEPTIONS
#endif
#ifndef FP_HANDLE_EXCEPTIONS
# define FP_HANDLE_EXCEPTIONS do {} while (0)
#endif
#ifndef FP_DENORM_ZERO
# define FP_DENORM_ZERO 0
#endif

#ifndef FP_INHIBIT_RESULTS
# define FP_INHIBIT_RESULTS 0
#endif

#define FP_SET_EXCEPTION(ex)				\
  _fex |= (ex)

#define FP_CUR_EXCEPTIONS				\
  (_fex)

#ifndef FP_TRAPPING_EXCEPTIONS
# define FP_TRAPPING_EXCEPTIONS 0
#endif

/* A file using soft-fp may define FP_NO_EXCEPTIONS before including
   soft-fp.h to indicate that, although a macro used there could raise
   exceptions, or do rounding and potentially thereby raise
   exceptions, for some arguments, for the particular arguments used
   in that file no exceptions or rounding can occur.  Such a file
   should not itself use macros relating to handling exceptions and
   rounding modes; this is only for indirect uses (in particular, in
   _FP_FROM_INT and the macros it calls).  */
#ifdef FP_NO_EXCEPTIONS

# undef FP_SET_EXCEPTION
# define FP_SET_EXCEPTION(ex) do {} while (0)

# undef FP_CUR_EXCEPTIONS
# define FP_CUR_EXCEPTIONS 0

# undef FP_TRAPPING_EXCEPTIONS
# define FP_TRAPPING_EXCEPTIONS 0

# undef FP_ROUNDMODE
# define FP_ROUNDMODE FP_RND_ZERO

# undef _FP_TININESS_AFTER_ROUNDING
# define _FP_TININESS_AFTER_ROUNDING 0

#endif

/* A file using soft-fp may define FP_NO_EXACT_UNDERFLOW before
   including soft-fp.h to indicate that, although a macro used there
   could allow for the case of exact underflow requiring the underflow
   exception to be raised if traps are enabled, for the particular
   arguments used in that file no exact underflow can occur.  */
#ifdef FP_NO_EXACT_UNDERFLOW
# undef FP_TRAPPING_EXCEPTIONS
# define FP_TRAPPING_EXCEPTIONS 0
#endif

#define _FP_ROUND_NEAREST(wc, X)				\
  do								\
    {								\
      if ((_FP_FRAC_LOW_##wc (X) & 15) != _FP_WORK_ROUND)	\
	_FP_FRAC_ADDI_##wc (X, _FP_WORK_ROUND);			\
    }								\
  while (0)

#define _FP_ROUND_ZERO(wc, X)		(void) 0

#define _FP_ROUND_PINF(wc, X)				\
  do							\
    {							\
      if (!X##_s && (_FP_FRAC_LOW_##wc (X) & 7))	\
	_FP_FRAC_ADDI_##wc (X, _FP_WORK_LSB);		\
    }							\
  while (0)

#define _FP_ROUND_MINF(wc, X)			\
  do						\
    {						\
      if (X##_s && (_FP_FRAC_LOW_##wc (X) & 7))	\
	_FP_FRAC_ADDI_##wc (X, _FP_WORK_LSB);	\
    }						\
  while (0)

#define _FP_ROUND(wc, X)			\
  do						\
    {						\
      if (_FP_FRAC_LOW_##wc (X) & 7)		\
	{					\
	  FP_SET_EXCEPTION (FP_EX_INEXACT);	\
	  switch (FP_ROUNDMODE)			\
	    {					\
	    case FP_RND_NEAREST:		\
	      _FP_ROUND_NEAREST (wc, X);	\
	      break;				\
	    case FP_RND_ZERO:			\
	      _FP_ROUND_ZERO (wc, X);		\
	      break;				\
	    case FP_RND_PINF:			\
	      _FP_ROUND_PINF (wc, X);		\
	      break;				\
	    case FP_RND_MINF:			\
	      _FP_ROUND_MINF (wc, X);		\
	      break;				\
	    }					\
	}					\
    }						\
  while (0)

#define FP_CLS_NORMAL		0
#define FP_CLS_ZERO		1
#define FP_CLS_INF		2
#define FP_CLS_NAN		3

#define _FP_CLS_COMBINE(x, y)	(((x) << 2) | (y))

#include "op-1.h"
#include "op-2.h"
#include "op-4.h"
#include "op-8.h"
#include "op-common.h"

#endif  /* _LOCAL_SOFT_FP_H_ */
