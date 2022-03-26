/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_CSKY_PGALLOC_H_
#define _ASM_CSKY_PGALLOC_H_

#include <bits.h>
#include <mm/gvm.h>
#include <asm/page.h>
#include <arch/csky/page.h>

typedef struct pte pte_t;
typedef struct pgd pgd_t;

#include <asm-generic/pgtable-nopmd.h>

#define SSEG0_BASE  0x80000000
#define SSEG1_BASE  0xa0000000

extern pgd_t page_dir[PTRS_PER_PGD];
extern pte_t invalid_pte_table[PTRS_PER_PTE];

static inline void pte_clear(pte_t *pte)
{
    pte->val = 0;
}

static inline bool pte_get_present(pte_t *pte)
{
    return pte->p || pte->g;
}

static inline bool pte_get_used(pte_t *pte)
{
    return !!pte->val;
}

static inline bool pte_get_dirty(pte_t *pte)
{
    return pte->d;
}

static inline void pte_set_dirty(pte_t *pte, bool dirty)
{
    pte->d = dirty;
}

static inline bool pte_get_accessed(pte_t *pte)
{
    return pte->a;
}

static inline void pte_set_accessed(pte_t *pte, bool accessed)
{
    pte->a = accessed;
}

static inline bool pte_get_wrprotect(pte_t *pte)
{
    return pte->w;
}

static inline void pte_set_wrprotect(pte_t *pte, bool wrprotect)
{
    pte->w = wrprotect;
}

#define CSKY_GENERIC_PGTABLE_OPS(name, type, valtype, child)                        \
static inline valtype name##_generic_pfn_mask(type *pgt)                            \
{                                                                                   \
    return PHYS_PAGE_MASK;                                                          \
}                                                                                   \
                                                                                    \
static inline valtype name##_generic_flags_mask(type *pgt)                          \
{                                                                                   \
    return ~name##_generic_pfn_mask(pgt);                                           \
}                                                                                   \
                                                                                    \
static inline valtype name##_generic_phys(type *pgt)                                \
{                                                                                   \
    return name##_generic_pfn_mask(pgt) & pgt->val;                                 \
}                                                                                   \
                                                                                    \
static inline valtype name##_generic_flags(type *pgt)                               \
{                                                                                   \
    return name##_generic_flags_mask(pgt) & pgt->val;                               \
}                                                                                   \
                                                                                    \
static inline size_t name##_generic_address(type *pgt)                              \
{                                                                                   \
    phys_addr_t phys;                                                               \
    phys = name##_generic_phys(pgt);                                                \
    return (size_t)pa_to_va(phys);                                                  \
}                                                                                   \
                                                                                    \
static inline void name##_generic_populate(type *pgt, child *cpt)                   \
{                                                                                   \
    pgt->val = va_to_pa(cpt);                                                       \
}                                                                                   \
                                                                                    \
static inline bool name##_generic_inval(type *pgt)                                  \
{                                                                                   \
    return name##_generic_flags(pgt);                                               \
}                                                                                   \
                                                                                    \
static inline void name##_generic_clear(type *pgt)                                  \
{                                                                                   \
    name##_generic_populate(pgt, invalid_pte_table);                                \
}                                                                                   \
                                                                                    \
static inline bool name##_generic_get_present(type *pgt)                            \
{                                                                                   \
    return (void *)name##_generic_address(pgt) != invalid_pte_table;                \
}                                                                                   \

CSKY_GENERIC_PGTABLE_OPS(pmd, pmd_t, pmdval_t, pte_t)
#define pmd_pfn_mask        pmd_generic_pfn_mask
#define pmd_flags_mask      pmd_generic_flags_mask
#define pmd_flags           pmd_generic_flags
#define pmd_address         pmd_generic_address
#define pmd_populate        pmd_generic_populate
#define pmd_inval           pmd_generic_inval
#define pmd_clear           pmd_generic_clear
#define pmd_get_present     pmd_generic_get_present

state arch_page_map(size_t pa, size_t va, size_t size);
void arch_page_setup(void);

#endif  /* _ASM_CSKY_PGALLOC_H_ */
