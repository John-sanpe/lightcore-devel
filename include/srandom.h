#ifndef _SRANDOM_H_
#define _SRANDOM_H_

#include <prandom.h>

struct srandom_state {
    unsigned long v0;
    unsigned long v1;
    unsigned long v2;
    unsigned long v3;
};

DECLARE_PERCPU(unsigned long, srandom_noise);
DECLARE_PERCPU(struct srandom_state, srandom_state);

#if BITS_PER_LONG == 64
# define SIPROUND(v0, v1, v2, v3) (     \
    v0 += v1, v1 = rol64(v1, 13),       \
    v2 += v3, v3 = rol64(v3, 16),       \
    v1 ^= v0, v0 = rol64(v0, 32),       \
    v3 ^= v2,                           \
    v0 += v3, v3 = rol64(v3, 21),       \
    v2 += v1, v1 = rol64(v1, 17),       \
    v3 ^= v0,                           \
    v1 ^= v2, v2 = rol64(v2, 32)        \
)
#else /* BITS_PER_LONG == 32 */
# define SIPROUND(v0, v1, v2, v3) (     \
    v0 += v1, v1 = rol32(v1,  5),       \
    v2 += v3, v3 = rol32(v3,  8),       \
    v1 ^= v0, v0 = rol32(v0, 16),       \
    v3 ^= v2,                           \
    v0 += v3, v3 = rol32(v3,  7),       \
    v2 += v1, v1 = rol32(v1, 13),       \
    v3 ^= v0,                           \
    v1 ^= v2, v2 = rol32(v2, 16)        \
)
#endif /* BITS_PER_LONG == 32 */

extern uint32_t srandom_value(void);
extern void srandom_byte(void *buff, size_t len);

#endif /* _SRANDOM_H_ */
