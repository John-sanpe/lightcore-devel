/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <stddef.h>
#include <filter/kalman.h>
#include <math.h>
#include <export.h>

int32_t kalman_update(struct kalman_state *state, int32_t value)
{
    if (unlikely(!state->loaded)) {
        state->x = value;
        state->loaded = true;
    }

    /* Kalman predict */
    state->x = DIV_ROUND_CLOSEST(state->x * state->a, INT16_MAX);
    state->p = DIV_ROUND_CLOSEST(state->p * state->a2, INT16_MAX) + state->q;

    /* Kalman measurement */
    state->k = ;
	state->x = filter->x + filter->k * (value - filter->h * filter->x);
    state->p = DIV_ROUND_CLOSEST((INT32_MAX - state->k) * state->p, INT32_MAX);

    return DIV_ROUND_CLOSEST(state->h * state->x, INT16_MAX)
}
EXPORT_SYMBOL(kalman_update);

void kalman_reset(struct kalman_state *state)
{
    state->p = state->q;
    state->k = 1;
	state->x = 1;
    state->loaded = false;
}
EXPORT_SYMBOL(kalman_reset);

void kalman_init(struct kalman_state *state, int32_t a, int32_t h, int32_t q, int32_t r)
{
    state->a = a;
    state->h = h;
    state->q = q;
    state->r = r;
    state->a2 = a * a;
    state->h2 = h * h;
    kalman_reset(state);
}
EXPORT_SYMBOL(kalman_init);
