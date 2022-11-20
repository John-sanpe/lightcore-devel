/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _FILTER_KALMAN_H_
#define _FILTER_KALMAN_H_

#include <types.h>
#include <limits.h>

/**
 *
 *
 */
struct kalman_state {
    /* const value */
    int32_t a, a2;
    int32_t h, h2;
    int32_t q;
    int32_t r;

    /* context value */
    int64_t x;
    int64_t p;
    int32_t k;
    bool loaded;
};

#define kalman_STATIC(WEIGHT) \
    {.weight = (WEIGHT)}

#define kalman_INIT(WEIGHT) \
    (struct kalman_state) kalman_STATIC(WEIGHT)

#define kalman_STATE(NAME, WEIGHT) \
    struct kalman_state NAME = kalman_INIT(WEIGHT)

extern int32_t kalman_update(struct kalman_state *state, int32_t value);
extern void kalman_reset(struct kalman_state *state);
extern void kalman_init(struct kalman_state *state, int32_t a, int32_t h, int32_t q, int32_t r);

#endif  /* _FILTER_KALMAN_H_ */
