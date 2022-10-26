#include <fmath.h>
#include <export.h>

static const double T[] = {
    0x15554d3418c99f.0p-54, /* 0.333331395030791399758 */
    0x1112fd38999f72.0p-55, /* 0.133392002712976742718 */
    0x1b54c91d865afe.0p-57, /* 0.0533812378445670393523 */
    0x191df3908c33ce.0p-58, /* 0.0245283181166547278873 */
    0x185dadfcecf44e.0p-61, /* 0.00297435743359967304927 */
    0x1362b9bf971bcd.0p-59, /* 0.00946564784943673166728 */
};

float __tandf(double x, int odd)
{
    double_t z, r, w, s, t, u;

    z = x * x;
    r = T[4] + z * T[5];
    t = T[2] + z * T[3];
    w = z * z;
    s = z * x;
    u = T[0] + z * T[1];
    r = (x + s * u) + (s * w) * (t + w * r);

    return odd ? -1.0 / r : r;
}
