/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <sort.h>
#include <string.h>
#include <export.h>

static __always_inline void
sort_swap(size_t cells, void *cela, void *celb)
{
    uint8_t buff[cells];

    memcpy(buff, cela, cells);
    memcpy(cela, celb, cells);
    memcpy(celb, buff, cells);
}

state bsort(void *base, size_t number, size_t cells, sort_cmp_t cmp, void *pdata)
{
    void *walk, *prev;
    unsigned int count;
    bool swapped;

    if (!base || number < 2 || !cells || !cmp)
        return -EINVAL;

    for (swapped = true; swapped &&
         ({swapped = false; true;}); --number) {
        prev = base;
        for (count = 1; count < number; ++count) {
            walk = base + cells * count;
            if (cmp(prev, walk, pdata) > 0) {
                sort_swap(cells, walk, prev);
                swapped = true;
            }
            prev = walk;
        }
    }

    return -ENOERR;
}
EXPORT_SYMBOL(bsort);
