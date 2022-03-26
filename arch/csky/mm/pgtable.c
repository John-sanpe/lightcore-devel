/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <mm/vmap.h>

state pte_set(pte_t *pte, phys_addr_t phys, gvm_t flags)
{
    pte->addr = phys >> PAGE_SHIFT;

    if (flags & GVM_NOCACHE)
        pte->cache = PAGE_UNCACHE;

    return -ENOERR;
}
