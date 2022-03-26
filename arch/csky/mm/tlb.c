/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <align.h>
#include <irqflags.h>
#include <asm/tlb.h>

static void restore_asid_inv_utlb(uint32_t oldpid, uint32_t newpid)
{
    if (oldpid == newpid)
        mmu_entryhi_set(oldpid + 1);
    mmu_entryhi_set(oldpid);
}

static void tlb_inval_page_pid(uintptr_t addr, uint32_t oldpid, uint32_t newpid)
{
    uint32_t index;

    mmu_entryhi_set(addr | newpid);
    mmu_cir_set(MCIR_TLBP);

    index = mmu_index_get();
    if (index >= 0)
        mmu_cir_set(MCIR_TLBINV);

    restore_asid_inv_utlb(oldpid, newpid);
}

static void tlb_inval_range_pid(uintptr_t start, uintptr_t end, uint32_t oldpid, uint32_t newpid)
{
    uint32_t index;

    while (start < end) {
        mmu_entryhi_set(start | newpid);
        mmu_cir_set(MCIR_TLBP);

        index = mmu_index_get();
        if (index >= 0)
            mmu_cir_set(MCIR_TLBINV);

        start += TLB_LINE_SIZE;
    }

    restore_asid_inv_utlb(oldpid, newpid);
}

void tlb_inval_page(uintptr_t addr)
{
    irqflags_t irqflags;
    uint32_t oldpid;

    addr = align_low(addr, TLB_LINE_SIZE);

    irqflags = irq_local_save();
    oldpid = mmu_entryhi_get() & MEH_ASID;

    tlb_inval_page_pid(addr, oldpid, oldpid);
    irq_local_restore(irqflags);
}

void tlb_inval_range(uintptr_t start, uintptr_t end)
{
    irqflags_t irqflags;
    uint32_t oldpid;

    start = align_low(start, TLB_LINE_SIZE);
    end = align_high(end, TLB_LINE_SIZE);

    irqflags = irq_local_save();
    oldpid = mmu_entryhi_get() & MEH_ASID;

    tlb_inval_range_pid(start, end, oldpid, oldpid);
    irq_local_restore(irqflags);
}
