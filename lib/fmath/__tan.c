#include <fmath.h>
#include <export.h>

static const double T[] = {
     3.33333333333334091986e-01, /* 3FD55555, 55555563 */
     1.33333333333201242699e-01, /* 3FC11111, 1110FE7A */
     5.39682539762260521377e-02, /* 3FABA1BA, 1BB341FE */
     2.18694882948595424599e-02, /* 3F9664F4, 8406D637 */
     8.86323982359930005737e-03, /* 3F8226E3, E96E8493 */
     3.59207910759131235356e-03, /* 3F6D6D22, C9560328 */
     1.45620945432529025516e-03, /* 3F57DBC8, FEE08315 */
     5.88041240820264096874e-04, /* 3F4344D8, F2F26501 */
     2.46463134818469906812e-04, /* 3F3026F7, 1A8D1068 */
     7.81794442939557092300e-05, /* 3F147E88, A03792A6 */
     7.14072491382608190305e-05, /* 3F12B80F, 32F0A7E9 */
    -1.85586374855275456654e-05, /* BEF375CB, DB605373 */
     2.59073051863633712884e-05, /* 3EFB2A70, 74BF7AD4 */
},
pio4 = 7.85398163397448278999e-01, /* 3FE921FB, 54442D18 */
pio4lo = 3.06161699786838301793e-17; /* 3C81A626, 33145C07 */

double __tan(double x, double y, int odd)
{
    double_t z, r, v, w, s, a;
    double w0, a0;
    uint32_t hx;
    int big, sign;

    GET_HIGH_WORD(hx, x);
    big = (hx & 0x7fffffff) >= 0x3FE59428;

    if (big) {
        sign = hx >> 31;
        if (sign)
        {
            x = -x;
            y = -y;
        }
        x = (pio4 - x) + (pio4lo - y);
        y = 0.0;
    }

    z = x * x;
    w = z * z;
    r = T[1] + w * (T[3] + w * (T[5] + w * (T[7] + w * (T[9] + w * T[11]))));
    v = z * (T[2] + w * (T[4] + w * (T[6] + w * (T[8] + w * (T[10] + w * T[12])))));
    s = z * x;
    r = y + z * (s * (r + v) + y) + s * T[0];
    w = x + r;

    if (big) {
        s = 1 - 2 * odd;
        v = s - 2.0 * (x + (r - w * w / (w + s)));
        return sign ? -v : v;
    }

    if (!odd)
        return w;

    w0 = w;
    SET_LOW_WORD(w0, 0);
    v = r - (w0 - x);
    a0 = a = -1.0 / w;
    SET_LOW_WORD(a0, 0);

    return a0 + a * (1.0 + a0 * w0 + a0 * v);
}
