/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _TIME_H_
#define _TIME_H_

#include <types.h>
#include <stddef.h>
#include <limits.h>

/* Parameters used to convert the timespec values */
#define MSEC_PER_SEC    1000UL
#define USEC_PER_MSEC   1000UL
#define NSEC_PER_USEC   1000UL
#define NSEC_PER_MSEC   1000000UL
#define USEC_PER_SEC    1000000UL
#define NSEC_PER_SEC    1000000000UL
#define PSEC_PER_SEC    1000000000000ULL
#define FSEC_PER_SEC    1000000000000000ULL

#define TTIME_MAX       INT64_MAX
#define TTIME_MIN       INT64_MIN

#define KTIME_MAX       INT64_MAX
#define KTIME_MIN       INT64_MIN
#define KTIME_SEC_MAX   (KTIME_MAX / NSEC_PER_SEC)
#define KTIME_SEC_MIN   (KTIME_MIN / NSEC_PER_SEC)

struct timespec {
    time_t tv_sec;
    long tv_nsec;
};

/**
 * timespec_compare - compares two timespec variables.
 * @timea: variables a.
 * @timeb: variables b.
 */
static inline int timespec_compare(const struct timespec *timea, const struct timespec *timeb)
{
    if (timea->tv_sec != timeb->tv_sec) {
        if (timea->tv_sec > timeb->tv_sec)
            return 1;
        else
            return -1;
    }
    if (timea->tv_nsec != timeb->tv_nsec) {
        if (timea->tv_nsec > timeb->tv_nsec)
            return 1;
        else
            return -1;
    }
    return 0;
}

/**
 * timespec_after - compare if a timespec value is bigger than another one.
 * @timea: the bigger one.
 * @timeb: the smaller one.
 */
static inline bool timespec_after(const struct timespec *timea, const struct timespec *timeb)
{
    return timespec_compare(timea, timeb) > 0;
}

/**
 * timespec_before - compare if a timespec value is smaller than another one.
 * @timea: the smaller one.
 * @timeb: the bigger one.
 */
static inline bool timespec_before(const struct timespec *timea, const struct timespec *timeb)
{
    return timespec_compare(timea, timeb) < 0;
}

/**
 * timespec_after - compare if a timespec value is bigger or equal than another one.
 * @timea: the bigger one.
 * @timeb: the smaller one.
 */
static inline bool timespec_after_equal(const struct timespec *timea, const struct timespec *timeb)
{
    return timespec_compare(timea, timeb) >= 0;
}

/**
 * timespec_before_equal - compare if a timespec value is smaller than or equal another one.
 * @timea: the smaller one.
 * @timeb: the bigger one.
 */
static inline bool timespec_before_equal(const struct timespec *timea, const struct timespec *timeb)
{
    return timespec_compare(timea, timeb) <= 0;
}

extern time_t mktime(unsigned int year, unsigned int mon, unsigned int day, unsigned int hour, unsigned int min, unsigned int sec);
extern struct timespec ns_to_timespec(const int64_t nsec);
extern int64_t timespec_to_ns(struct timespec *ts);

extern bool timespec_valid(struct timespec *ts);
extern void timespec_normalized(struct timespec *ts);
extern void timespec_set_normalized(struct timespec *ts, time_t sec, int64_t nsec);
extern struct timespec timespec_add(struct timespec *lts, struct timespec *rts);
extern struct timespec timespec_sub(struct timespec *lts, struct timespec *rts);

#endif  /* _TIME_H_ */
