#ifndef _LOCAL_DOUBLE_H
#define _LOCAL_DOUBLE_H

#include <libgcc.h>

#if BITS_PER_LONG < 64
#define _FP_FRACTBITS_D        (2 * BITS_PER_LONG)
#define _FP_FRACTBITS_DW_D     (4 * BITS_PER_LONG)
#else
#define _FP_FRACTBITS_D        BITS_PER_LONG
#define _FP_FRACTBITS_DW_D     (2 * BITS_PER_LONG)
#endif

#define _FP_FRACBITS_D          53
#define _FP_FRACXBITS_D         (_FP_FRACTBITS_D - _FP_FRACBITS_D)
#define _FP_WFRACBITS_D         (_FP_WORKBITS + _FP_FRACBITS_D)
#define _FP_WFRACXBITS_D        (_FP_FRACTBITS_D - _FP_WFRACBITS_D)
#define _FP_EXPBITS_D           11
#define _FP_EXPBIAS_D           1023
#define _FP_EXPMAX_D            2047

#define _FP_QNANBIT_D           ((_FP_W_TYPE) 1 << (_FP_FRACBITS_D-2) % BITS_PER_LONG)
#define _FP_QNANBIT_SH_D        ((_FP_W_TYPE) 1 << (_FP_FRACBITS_D-2+_FP_WORKBITS) % BITS_PER_LONG)
#define _FP_IMPLBIT_D           ((_FP_W_TYPE) 1 << (_FP_FRACBITS_D-1) % BITS_PER_LONG)
#define _FP_IMPLBIT_SH_D        ((_FP_W_TYPE) 1 << (_FP_FRACBITS_D-1+_FP_WORKBITS) % BITS_PER_LONG)
#define _FP_OVERFLOW_D          ((_FP_W_TYPE) 1 << _FP_WFRACBITS_D % BITS_PER_LONG)
#define _FP_WFRACBITS_DW_D      (2 * _FP_WFRACBITS_D)
#define _FP_WFRACXBITS_DW_D     (_FP_FRACTBITS_DW_D - _FP_WFRACBITS_DW_D)
#define _FP_HIGHBIT_DW_D        ((_FP_W_TYPE) 1 << (_FP_WFRACBITS_DW_D - 1) % BITS_PER_LONG)

#if BITS_PER_LONG < 64
union _FP_UNION_D {
    DFtype flt;
    struct {
#ifdef ARCH_BIG_ENDIAN
        unsigned sign  : 1;
        unsigned exp   : _FP_EXPBITS_D;
        unsigned frac1 : _FP_FRACBITS_D - (_FP_IMPLBIT_D != 0) - BITS_PER_LONG;
        unsigned frac0 : BITS_PER_LONG;
#else
        unsigned frac0 : BITS_PER_LONG;
        unsigned frac1 : _FP_FRACBITS_D - (_FP_IMPLBIT_D != 0) - BITS_PER_LONG;
        unsigned exp   : _FP_EXPBITS_D;
        unsigned sign  : 1;
#endif
    } bits;
};

#define FP_DECL_D(X)               _FP_DECL(2, X)
#define FP_UNPACK_RAW_D(X, val)    _FP_UNPACK_RAW_2(D, X, (val))
#define FP_UNPACK_RAW_DP(X, val)   _FP_UNPACK_RAW_2_P(D, X, (val))
#define FP_PACK_RAW_D(val, X)      _FP_PACK_RAW_2(D, (val), X)

#define FP_PACK_RAW_DP(val, X) do {         \
    if (!FP_INHIBIT_RESULTS)                \
        _FP_PACK_RAW_2_P(D, (val), X);      \
} while (0)

#define FP_UNPACK_D(X, val) do {            \
    _FP_UNPACK_RAW_2(D, X, (val));          \
    _FP_UNPACK_CANONICAL(D, 2, X);          \
} while (0)

#define FP_UNPACK_DP(X, val) do {           \
    _FP_UNPACK_RAW_2_P(D, X, (val));        \
    _FP_UNPACK_CANONICAL(D, 2, X);          \
} while (0)

#define FP_UNPACK_SEMIRAW_D(X, val) do {    \
    _FP_UNPACK_RAW_2(D, X, (val));          \
    _FP_UNPACK_SEMIRAW(D, 2, X);            \
} while (0)

#define FP_UNPACK_SEMIRAW_DP(X, val) do {   \
    _FP_UNPACK_RAW_2_P(D, X, (val));        \
    _FP_UNPACK_SEMIRAW(D, 2, X);            \
} while (0)

#define FP_PACK_D(val, X) do {              \
    _FP_PACK_CANONICAL(D, 2, X);            \
    _FP_PACK_RAW_2(D, (val), X);            \
} while (0)

#define FP_PACK_DP(val, X) do {             \
    _FP_PACK_CANONICAL(D, 2, X);            \
    if (!FP_INHIBIT_RESULTS)                \
        _FP_PACK_RAW_2_P(D, (val), X);      \
} while (0)

#define FP_PACK_SEMIRAW_D(val, X) do {      \
      _FP_PACK_SEMIRAW(D, 2, X);            \
      _FP_PACK_RAW_2(D, (val), X);          \
} while (0)

#define FP_PACK_SEMIRAW_DP(val, X) do {     \
    _FP_PACK_SEMIRAW(D, 2, X);              \
    if (!FP_INHIBIT_RESULTS)                \
        _FP_PACK_RAW_2_P(D, (val), X);      \
} while (0)

#define FP_ISSIGNAN_D(X)                   _FP_ISSIGNAN(D, 2, X)
#define FP_NEG_D(R, X)                     _FP_NEG(D, 2, R, X)
#define FP_ADD_D(R, X, Y)                  _FP_ADD(D, 2, R, X, Y)
#define FP_SUB_D(R, X, Y)                  _FP_SUB(D, 2, R, X, Y)
#define FP_MUL_D(R, X, Y)                  _FP_MUL(D, 2, R, X, Y)
#define FP_DIV_D(R, X, Y)                  _FP_DIV(D, 2, R, X, Y)
#define FP_SQRT_D(R, X)                    _FP_SQRT(D, 2, R, X)
#define _FP_SQRT_MEAT_D(R, S, T, X, Q)     _FP_SQRT_MEAT_2(R, S, T, X, (Q))
#define FP_FMA_D(R, X, Y, Z)               _FP_FMA(D, 2, 4, R, X, Y, Z)
#define FP_CMP_D(r, X, Y, un, ex)          _FP_CMP(D, 2, (r), X, Y, (un), (ex))
#define FP_CMP_EQ_D(r, X, Y, ex)           _FP_CMP_EQ(D, 2, (r), X, Y, (ex))
#define FP_CMP_UNORD_D(r, X, Y, ex)        _FP_CMP_UNORD(D, 2, (r), X, Y, (ex))
#define FP_TO_INT_D(r, X, rsz, rsg)        _FP_TO_INT(D, 2, (r), X, (rsz), (rsg))
#define FP_TO_INT_ROUND_D(r, X, rsz, rsg)  _FP_TO_INT_ROUND(D, 2, (r), X, (rsz), (rsg))
#define FP_FROM_INT_D(X, r, rs, rt)        _FP_FROM_INT(D, 2, X, (r), (rs), rt)
#define _FP_FRAC_HIGH_D(X)                 _FP_FRAC_HIGH_2(X)
#define _FP_FRAC_HIGH_RAW_D(X)             _FP_FRAC_HIGH_2(X)
#define _FP_FRAC_HIGH_DW_D(X)              _FP_FRAC_HIGH_4(X)

#else

union _FP_UNION_D {
    DFtype flt;
    struct _FP_STRUCT_LAYOUT {
#ifdef ARCH_BIG_ENDIAN
        unsigned sign   : 1;
        unsigned exp    : _FP_EXPBITS_D;
        _FP_W_TYPE frac : _FP_FRACBITS_D - (_FP_IMPLBIT_D != 0);
#else
        _FP_W_TYPE frac : _FP_FRACBITS_D - (_FP_IMPLBIT_D != 0);
        unsigned exp    : _FP_EXPBITS_D;
        unsigned sign   : 1;
#endif
    } bits;
};

#define FP_DECL_D(X)                _FP_DECL(1, X)
#define FP_UNPACK_RAW_D(X, val)     _FP_UNPACK_RAW_1(D, X, (val))
#define FP_UNPACK_RAW_DP(X, val)    _FP_UNPACK_RAW_1_P(D, X, (val))
#define FP_PACK_RAW_D(val, X)       _FP_PACK_RAW_1(D, (val), X)

#define FP_PACK_RAW_DP(val, X) do {         \
    if (!FP_INHIBIT_RESULTS)                \
        _FP_PACK_RAW_1_P(D, (val), X);      \
} while (0)

#define FP_UNPACK_D(X, val) do {            \
    _FP_UNPACK_RAW_1(D, X, (val));          \
    _FP_UNPACK_CANONICAL(D, 1, X);          \
} while (0)

#define FP_UNPACK_DP(X, val) do {           \
    _FP_UNPACK_RAW_1_P(D, X, (val));        \
    _FP_UNPACK_CANONICAL(D, 1, X);          \
} while (0)

#define FP_UNPACK_SEMIRAW_D(X, val) do {    \
    _FP_UNPACK_RAW_1(D, X, (val));          \
    _FP_UNPACK_SEMIRAW(D, 1, X);            \
} while (0)

#define FP_UNPACK_SEMIRAW_DP(X, val) do {   \
    _FP_UNPACK_RAW_1_P(D, X, (val));        \
    _FP_UNPACK_SEMIRAW(D, 1, X);            \
} while (0)

#define FP_PACK_D(val, X) do {              \
    _FP_PACK_CANONICAL(D, 1, X);            \
    _FP_PACK_RAW_1(D, (val), X);            \
} while (0)

#define FP_PACK_DP(val, X) do {             \
    _FP_PACK_CANONICAL(D, 1, X);            \
    if (!FP_INHIBIT_RESULTS)                \
        _FP_PACK_RAW_1_P(D, (val), X);      \
} while (0)

#define FP_PACK_SEMIRAW_D(val, X) do {      \
    _FP_PACK_SEMIRAW(D, 1, X);              \
    _FP_PACK_RAW_1(D, (val), X);            \
} while (0)

#define FP_PACK_SEMIRAW_DP(val, X) do {     \
    _FP_PACK_SEMIRAW(D, 1, X);              \
    if (!FP_INHIBIT_RESULTS)                \
        _FP_PACK_RAW_1_P(D, (val), X);      \
} while (0)

#define FP_ISSIGNAN_D(X)                    _FP_ISSIGNAN(D, 1, X)
#define FP_NEG_D(R, X)                      _FP_NEG(D, 1, R, X)
#define FP_ADD_D(R, X, Y)                   _FP_ADD(D, 1, R, X, Y)
#define FP_SUB_D(R, X, Y)                   _FP_SUB(D, 1, R, X, Y)
#define FP_MUL_D(R, X, Y)                   _FP_MUL(D, 1, R, X, Y)
#define FP_DIV_D(R, X, Y)                   _FP_DIV(D, 1, R, X, Y)
#define FP_SQRT_D(R, X)                     _FP_SQRT(D, 1, R, X)
#define _FP_SQRT_MEAT_D(R, S, T, X, Q)      _FP_SQRT_MEAT_1(R, S, T, X, (Q))
#define FP_FMA_D(R, X, Y, Z)                _FP_FMA(D, 1, 2, R, X, Y, Z)
#define FP_CMP_D(r, X, Y, un, ex)           _FP_CMP(D, 1, (r), X, Y, (un), (ex))
#define FP_CMP_EQ_D(r, X, Y, ex)            _FP_CMP_EQ(D, 1, (r), X, Y, (ex))
#define FP_CMP_UNORD_D(r, X, Y, ex)         _FP_CMP_UNORD(D, 1, (r), X, Y, (ex))
#define FP_TO_INT_D(r, X, rsz, rsg)         _FP_TO_INT(D, 1, (r), X, (rsz), (rsg))
#define FP_TO_INT_ROUND_D(r, X, rsz, rsg)   _FP_TO_INT_ROUND(D, 1, (r), X, (rsz), (rsg))
#define FP_FROM_INT_D(X, r, rs, rt)         _FP_FROM_INT(D, 1, X, (r), (rs), rt)
#define _FP_FRAC_HIGH_D(X)                  _FP_FRAC_HIGH_1(X)
#define _FP_FRAC_HIGH_RAW_D(X)              _FP_FRAC_HIGH_1(X)
#define _FP_FRAC_HIGH_DW_D(X)               _FP_FRAC_HIGH_2(X)

#endif  /* W_TYPE_SIZE < 64 */
#endif  /* !SOFT_FP_DOUBLE_H */