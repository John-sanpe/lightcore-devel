/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <srandom.h>
#include <cpu.h>
#include <timer.h>
#include <initcall.h>
#include <bytefill.h>
#include <export.h>

static void srandom_reseed(void *pdata);
static DEFINE_TIMER(srandom_seeder, srandom_reseed, NULL, 0, 0);

DEFINE_PERCPU(unsigned long, srandom_noise);
DEFINE_PERCPU(struct srandom_state, srandom_state);

EXPORT_SYMBOL(srandom_noise);
EXPORT_SYMBOL(srandom_state);

static uint32_t sstate_u32(struct srandom_state *state)
{
    unsigned long noise = thiscpu(srandom_noise);
    unsigned long v0, v1, v2, v3;

    v0 = state->v0;
    v1 = state->v1;
    v2 = state->v2;
    v3 = state->v3;

    v3 ^= noise;
    SIPROUND(v0, v1, v2, v3);
    SIPROUND(v0, v1, v2, v3);
    v0 ^= noise;

    state->v0 = v0;
    state->v1 = v1;
    state->v2 = v2;
    state->v3 = v3;

    return v1 + v3;
}

uint32_t srandom_value(void)
{
    struct srandom_state *pstate = thiscpu_ptr(&srandom_state);
    return sstate_u32(pstate);
}
EXPORT_SYMBOL(srandom_value);

BYTEFILL_DECLARE_ARGS(,
    srandom_bytes, uint32_t,
    srandom_value, ,
    BYTEFILL_FIXED_ARGS
)
EXPORT_SYMBOL(srandom_bytes);

void srandom_seed(uint64_t seed)
{
    struct srandom_state *pstate;
    unsigned int cpu;

    cpu_for_each_possible(cpu) {
        pstate = percpu_ptr(cpu, &srandom_state);
        srandom_state_seed(pstate, seed);
    }
}
EXPORT_SYMBOL(srandom_seed);

static void srandom_reseed(void *pdata)
{


}

static state srandom_init(void)
{
    srandom_seeder.delta = ticktime;
    timer_pending(&srandom_seeder);
}
late_initcall(srandom_init);
