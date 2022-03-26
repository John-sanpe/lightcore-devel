/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_CSKY_REGS_H_
#define _ASM_CSKY_REGS_H_

#include <types.h>
#include <arch/csky/regs.h>

#ifndef __ASSEMBLY__

#ifdef CONFIG_CSKY_ABIV1

struct regs {
    uint32_t psr;
    uint32_t pc;
    uint32_t sp;
    uint32_t r1;

    uint32_t a0;
    uint32_t a1;
    uint32_t a2;
    uint32_t a3;
    uint32_t a4;
    uint32_t a5;

    uint32_t l0;
    uint32_t l1;
    uint32_t l2;
    uint32_t l3;
    uint32_t l4;
    uint32_t l5;

    uint32_t gb;
    uint32_t lr;
} __packed;

#endif  /* CONFIG_CSKY_ABIV1 */

#ifndef __SYMBOL__

#define mfcr(reg) ({                        \
    unsigned int tmp;                       \
    asm volatile (                          \
        "mfcr %0, "reg"\n"                  \
        : "=r"(tmp)                         \
        :: "memory"                         \
    );                                      \
    tmp;                                    \
})

#define mtcr(reg, val) ({                   \
    asm volatile (                          \
        "mtcr %0, "reg"\n"                  \
        :: "r"(val)                         \
        : "memory"                          \
    );                                      \
})

#define MFCR_GET_OP(reg, name)              \
static __always_inline size_t               \
generic_##name##_get(void)                  \
{                                           \
    size_t tmp;                             \
                                            \
    asm volatile (                          \
        "mtcr %0, "reg"\n"                  \
        :: "r"(tmp)                         \
        : "memory"                          \
    );                                      \
                                            \
    return tmp;                             \
}

#define MTCR_SET_OP(reg, name)              \
static __always_inline void                 \
generic_##name##_set(size_t value)          \
{                                           \
    asm volatile (                          \
        "mtcr %0, "reg"\n"                  \
        :: "r"(value)                       \
        : "memory"                          \
    );                                      \
}

MFCR_GET_OP("cr0", psr)
MFCR_GET_OP("cr1", vbr)
MFCR_GET_OP("cr6", ss0)
MFCR_GET_OP("cr7", ss1)
MFCR_GET_OP("cr8", ss2)
MFCR_GET_OP("cr9", ss3)
MFCR_GET_OP("cr10", ss4)
MFCR_GET_OP("cr11", gcr)
MFCR_GET_OP("cr12", gsr)
MFCR_GET_OP("cr13", cpuidrr)
MFCR_GET_OP("cr14", dscr)
MFCR_GET_OP("cr15", cpsr)
MFCR_GET_OP("cr17", cfr)
MFCR_GET_OP("cr18", ccr)
MFCR_GET_OP("cr19", capr)
MFCR_GET_OP("cr20", pacr)
MFCR_GET_OP("cr21", prse)
MFCR_GET_OP("cr22", cir)
MFCR_GET_OP("cr30", chr)

MTCR_SET_OP("cr0", psr)
MTCR_SET_OP("cr1", vbr)
MTCR_SET_OP("cr6", ss0)
MTCR_SET_OP("cr7", ss1)
MTCR_SET_OP("cr8", ss2)
MTCR_SET_OP("cr9", ss3)
MTCR_SET_OP("cr10", ss4)
MTCR_SET_OP("cr11", gcr)
MTCR_SET_OP("cr12", gsr)
MTCR_SET_OP("cr13", cpuidrr)
MTCR_SET_OP("cr14", dscr)
MTCR_SET_OP("cr15", cpsr)
MTCR_SET_OP("cr17", cfr)
MTCR_SET_OP("cr18", ccr)
MTCR_SET_OP("cr19", capr)
MTCR_SET_OP("cr20", pacr)
MTCR_SET_OP("cr21", prse)
MTCR_SET_OP("cr22", cir)
MTCR_SET_OP("cr30", chr)

#define psr_get         generic_psr_get
#define vbr_get         generic_vbr_get
#define ss0_get         generic_ss0_get
#define ss1_get         generic_ss1_get
#define ss2_get         generic_ss2_get
#define ss3_get         generic_ss3_get
#define ss4_get         generic_ss4_get
#define gcr_get         generic_gcr_get
#define gsr_get         generic_gsr_get
#define cpuidrr_get     generic_cpuidrr_get
#define dscr_get        generic_dscr_get
#define cpsr_get        generic_cpsr_get
#define cfr_get         generic_cfr_get
#define ccr_get         generic_ccr_get
#define capr_get        generic_capr_get
#define pacr_get        generic_pacr_get
#define prse_get        generic_prse_get
#define cir_get         generic_cir_get
#define chr_get         generic_chr_get

#define psr_set         generic_psr_set
#define vbr_set         generic_vbr_set
#define ss0_set         generic_ss0_set
#define ss1_set         generic_ss1_set
#define ss2_set         generic_ss2_set
#define ss3_set         generic_ss3_set
#define ss4_set         generic_ss4_set
#define gcr_set         generic_gcr_set
#define gsr_set         generic_gsr_set
#define cpuidrr_set     generic_cpuidrr_set
#define dscr_set        generic_dscr_set
#define cpsr_set        generic_cpsr_set
#define cfr_set         generic_cfr_set
#define ccr_set         generic_ccr_set
#define capr_set        generic_capr_set
#define pacr_set        generic_pacr_set
#define prse_set        generic_prse_set
#define cir_set         generic_cir_set
#define chr_set         generic_chr_set

#ifdef CONFIG_CSKY_ABIV1

#define cprcr(reg) ({                       \
    unsigned int tmp;                       \
    asm volatile (                          \
        "cprcr %0, "reg"\n"                 \
        : "=b"(tmp)                         \
    );                                      \
    tmp;                                    \
})

#define cpwcr(reg, val) ({                  \
    asm volatile (                          \
        "cpwcr %0, "reg"\n"                 \
        :: "b"(val)                         \
    );                                      \
})

#define CPRCR_GET_OP(reg, name)             \
static __always_inline size_t               \
generic_##name##_get(void)                  \
{                                           \
    size_t tmp;                             \
                                            \
    asm volatile (                          \
        "cprcr %0, "reg"\n"                 \
        : "=b"(tmp)                         \
    );                                      \
                                            \
    return tmp;                             \
}

#define CPRCR_SET_OP(reg, name)             \
static __always_inline void                 \
generic_##name##_set(size_t value)          \
{                                           \
    asm volatile (                          \
        "cpwcr %0, "reg"\n"                 \
        :: "b"(value)                       \
    );                                      \
}

CPRCR_GET_OP("cpcr0", mir)
CPRCR_GET_OP("cpcr1", mrr)
CPRCR_GET_OP("cpcr2", me0)
CPRCR_GET_OP("cpcr3", me1)
CPRCR_GET_OP("cpcr4", meh)
CPRCR_GET_OP("cpcr5", mcr)
CPRCR_GET_OP("cpcr6", mpr)
CPRCR_GET_OP("cpcr7", mwr)
CPRCR_GET_OP("cpcr8", mcir)
CPRCR_GET_OP("cpcr29", mpgd)
CPRCR_GET_OP("cpcr30", msa0)
CPRCR_GET_OP("cpcr31", msa1)

CPRCR_SET_OP("cpcr0", mir)
CPRCR_SET_OP("cpcr1", mrr)
CPRCR_SET_OP("cpcr2", me0)
CPRCR_SET_OP("cpcr3", me1)
CPRCR_SET_OP("cpcr4", meh)
CPRCR_SET_OP("cpcr5", mcr)
CPRCR_SET_OP("cpcr6", mpr)
CPRCR_SET_OP("cpcr7", mwr)
CPRCR_SET_OP("cpcr8", mcir)
CPRCR_SET_OP("cpcr29", mpgd)
CPRCR_SET_OP("cpcr30", msa0)
CPRCR_SET_OP("cpcr31", msa1)

#define mmu_index_get       generic_mir_get
#define mmu_entrylo0_get    generic_me0_get
#define mmu_entrylo1_get    generic_me1_get
#define mmu_entryhi_get     generic_meh_get
#define mmu_pagemask_get    generic_mpr_get
#define mmu_cir_get         generic_mcir_get
#define mmu_pgd_get         generic_mpgd_get
#define mmu_msa0_get        generic_msa0_get
#define mmu_msa1_get        generic_msa1_get

#define mmu_index_set       generic_mir_set
#define mmu_entrylo0_set    generic_me0_set
#define mmu_entrylo1_set    generic_me1_set
#define mmu_entryhi_set     generic_meh_set
#define mmu_pagemask_set    generic_mpr_set
#define mmu_cir_set         generic_mcir_set
#define mmu_pgd_set         generic_mpgd_set
#define mmu_msa0_set        generic_msa0_set
#define mmu_msa1_set        generic_msa1_set

#endif  /* CONFIG_CSKY_ABIV1 */
#endif  /* __SYMBOL__ */
#endif  /* __ASSEMBLY__ */
#endif  /* __ASM_REGS_OPS_H */
