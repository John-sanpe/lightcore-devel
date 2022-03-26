/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_CSKY_TLB_H_
#define _ASM_CSKY_TLB_H_

#include <asm/page.h>
#include <asm/regs.h>

#define TLB_LINE_SHIFT      (PAGE_SHIFT + 1)
#define TLB_LINE_SIZE       (_AC(1,UL) << PGDIR_SHIFT)
#define TLB_LINE_MASK       (~(PGDIR_SIZE - 1))

#ifdef CONFIG_CSKY_ABIV1

struct memory;

static inline void tlb_inval_all(void)
{
    cpwcr("cpcr8", MCIR_TLBINVA);
}

#endif  /* CONFIG_CSKY_ABIV1 */

extern void tlb_inval_page(uintptr_t page);
extern void tlb_inval_range(uintptr_t start, uintptr_t end);

extern void tlb_inval_mm(struct memory *mm);
extern void tlb_inval_mm_page(struct memory *mm, uintptr_t page);
extern void tlb_inval_mm_range(struct memory *mm, uintptr_t start, uintptr_t end);

#endif  /* _ASM_CSKY_TLBFLUSH_H_ */
