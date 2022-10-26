/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _FMATH_H_
#define _FMATH_H_

#include <types.h>
#include <limits.h>
#include <float.h>

#define FORCE_EVAL(x) do {                          \
    if (sizeof(x) == sizeof(float)) {               \
        volatile float __x __always_unused;         \
        __x = (x);                                  \
    } else if (sizeof(x) == sizeof(double)) {       \
        volatile double __x __always_unused;        \
        __x = (x);                                  \
    } else {                                        \
        volatile long double __x __always_unused;   \
        __x = (x);                                  \
    }                                               \
} while(0)

#define asuint(f)				((union {float _f; uint32_t _i;}){f})._i
#define asfloat(i)				((union {uint32_t _i; float _f;}){i})._f
#define asuint64(f)				((union {double _f; uint64_t _i;}){f})._i
#define asdouble(i)				((union {uint64_t _i; double _f;}){i})._f

#define EXTRACT_WORDS(hi, lo, d) \
do { \
    uint64_t __u = asuint64(d); \
    (hi) = __u >> 32; \
    (lo) = (uint32_t)__u; \
} while(0)

#define GET_HIGH_WORD(hi, d) \
do { \
    (hi) = asuint64(d) >> 32; \
} while(0)

#define GET_LOW_WORD(lo, d) \
do { \
    (lo) = (uint32_t)asuint64(d); \
} while(0)

#define INSERT_WORDS(d, hi, lo) \
do { \
    (d) = asdouble(((uint64_t)(hi) << 32) | (uint32_t)(lo)); \
} while(0)

#define SET_HIGH_WORD(d, hi) \
    INSERT_WORDS(d, hi, (uint32_t)asuint64(d))

#define SET_LOW_WORD(d, lo) \
    INSERT_WORDS(d, asuint64(d) >> 32, lo)

#define GET_FLOAT_WORD(w, d) \
do { \
    (w) = asuint(d); \
} while(0)

#define SET_FLOAT_WORD(d, w) \
do { \
    (d) = asfloat(w); \
} while(0)

#define FP_NAN					0
#define FP_INFINITE				1
#define FP_ZERO					2
#define FP_SUBNORMAL			3
#define FP_NORMAL				4

#define M_E                     2.7182818284590452354	/* e */
#define M_LOG2E                 1.4426950408889634074	/* log_2 e */
#define M_LOG10E                0.43429448190325182765	/* log_10 e */
#define M_LN2                   0.69314718055994530942	/* log_e 2 */
#define M_LN10                  2.30258509299404568402	/* log_e 10 */
#define M_PI                    3.14159265358979323846	/* pi */
#define M_PI_2                  1.57079632679489661923	/* pi/2 */
#define M_PI_4                  0.78539816339744830962	/* pi/4 */
#define M_1_PI                  0.31830988618379067154	/* 1/pi */
#define M_2_PI                  0.63661977236758134308	/* 2/pi */
#define M_2_SQRTPI              1.12837916709551257390	/* 2/sqrt(pi) */
#define M_SQRT2                 1.41421356237309504880	/* sqrt(2) */
#define M_SQRT1_2               0.70710678118654752440	/* 1/sqrt(2) */

#define NAN						__builtin_nan("")
#define INFINITY				__builtin_inf()
#define	HUGE_VALF				__builtin_huge_valf()
#define	HUGE_VAL				__builtin_huge_val()
#define	HUGE_VALL				__builtin_huge_vall()

#define	isgreater(x, y)			__builtin_isgreater((x), (y))
#define	isgreaterequal(x, y)	__builtin_isgreaterequal((x), (y))
#define	isless(x, y)			__builtin_isless((x), (y))
#define	islessequal(x, y)		__builtin_islessequal((x), (y))
#define	islessgreater(x, y)		__builtin_islessgreater((x), (y))
#define	isunordered(x, y)		__builtin_isunordered((x), (y))
#define predict_true(x)			__builtin_expect(!!(x), 1)
#define predict_false(x)		__builtin_expect(x, 0)

static inline unsigned __FLOAT_BITS(float __f)
{
    union {float __f; unsigned __i;} __u;
    __u.__f = __f;
    return __u.__i;
}

static inline unsigned long long __DOUBLE_BITS(double __f)
{
    union {double __f; unsigned long long __i;} __u;
    __u.__f = __f;
    return __u.__i;
}

#define fpclassify(x) ( \
    sizeof(x) == sizeof(float) ? __fpclassifyf(x) : \
    sizeof(x) == sizeof(double) ? __fpclassify(x) : \
    __fpclassify(x) )

#define isinf(x) ( \
    sizeof(x) == sizeof(float) ? (__FLOAT_BITS(x) & 0x7fffffff) == 0x7f800000 : \
    sizeof(x) == sizeof(double) ? (__DOUBLE_BITS(x) & -1ULL >> 1) == 0x7ffULL << 52 : \
    __fpclassify(x) == FP_INFINITE)

#define isnan(x) ( \
    sizeof(x) == sizeof(float) ? (__FLOAT_BITS(x) & 0x7fffffff) > 0x7f800000 : \
    sizeof(x) == sizeof(double) ? (__DOUBLE_BITS(x) & -1ULL >> 1) > 0x7ffULL << 52 : \
    __fpclassify(x) == FP_NAN)

#define isnormal(x) ( \
    sizeof(x) == sizeof(float) ? ((__FLOAT_BITS(x) + 0x00800000) & 0x7fffffff) >= 0x01000000 : \
    sizeof(x) == sizeof(double) ? ((__DOUBLE_BITS(x) + (1ULL << 52)) & -1ULL >> 1) >= 1ULL << 53 : \
    __fpclassify(x) == FP_NORMAL)

#define isfinite(x) ( \
    sizeof(x) == sizeof(float) ? (__FLOAT_BITS(x) & 0x7fffffff) < 0x7f800000 : \
    sizeof(x) == sizeof(double) ? (__DOUBLE_BITS(x) & -1ULL >> 1) < 0x7ffULL << 52 : \
    __fpclassify(x) > FP_INFINITE)

#define signbit(x) ( \
    sizeof(x) == sizeof(float) ? (int)(__FLOAT_BITS(x) >> 31) : \
    sizeof(x) == sizeof(double) ? (int)(__DOUBLE_BITS(x) >> 63) : \
    (int)(__DOUBLE_BITS(x) >> 63))

extern double __cos(double x, double y);
extern float __cosdf(double x);
extern double __expo2(double x, double y);
extern float __expo2f(float x, float y);
extern int __fpclassify(double x);
extern int __fpclassifyf(float x);
extern int __rem_pio2_large(double *x, double *y, int z, int a, int b);
extern int __rem_pio2(double x, double *y);
extern int __rem_pio2f( float x, double *y);
extern double __sin(double x, double x, int y);
extern float __sindf(double x);
extern double __tan(double x, double x, int y);
extern float __tandf(double x, int y);

extern float acosf(float x);
extern float acoshf(float x);
extern float asinf(float x);
extern float asinhf(float x);
extern float atanf(float x);
extern float atan2f(float x, float y);
extern float atanhf(float x);
extern float cbrtf(float x);
extern float ceilf(float x);
extern float cosf(float x);
extern float coshf(float x);
extern float erff(float x);
extern float erfcf(float x);
extern float expf(float x);
extern float exp2f(float x);
extern float expm1f(float x);
extern float fabsf(float x);
extern float fdimf(float x, float y);
extern float floorf(float x);
extern float fmaxf(float x, float y);
extern float fminf(float x, float y);
extern float fmodf(float x, float y);
extern float frexpf(float x, int *y);
extern float hypotf(float x, float y);
extern float ldexpf(float x, int y);
extern float logf(float x);
extern float log10f(float x);
extern float log1pf(float x);
extern float log2f(float x);
extern float modff(float x, float *y);
extern float powf(float x, float y);
extern float rintf(float x);
extern float roundf(float x);
extern float scalbnf(float x, int y);
extern float scalblnf(float x, long y);
extern float sinf(float x);
extern float sinhf(float x);
extern float sqrtf(float x);
extern float tanf(float x);
extern float tanhf(float x);
extern float truncf(float x);

extern double acos(double x);
extern double acosh(double x);
extern double asin(double x);
extern double asinh(double x);
extern double atan(double x);
extern double atan2(double x, double y);
extern double atanh(double x);
extern double cbrt(double x);
extern double ceil(double x);
extern double cos(double x);
extern double cosh(double x);
extern double erf(double x);
extern double erfc(double x);
extern double exp(double x);
extern double exp2(double x);
extern double expm1(double x);
extern double fabs(double x);
extern double fdim(double x, double y);
extern double floor(double x);
extern double fmax(double x, double y);
extern double fmin(double x, double y);
extern double fmod(double x, double y);
extern double frexp(double x, int *y);
extern double hypot(double x, double);
extern double ldexp(double x, int y);
extern double log(double x);
extern double log10(double x);
extern double log1p(double x);
extern double log2(double x);
extern double modf(double x, double *);
extern double pow(double x, double);
extern double rint(double x);
extern double round(double x);
extern double scalbn(double x, int y);
extern double scalbln(double x, long);
extern double sin(double x);
extern double sinh(double x);
extern double sqrt(double x);
extern double tan(double x);
extern double tanh(double x);
extern double trunc(double x);

#endif /* _FMATH_H_ */
