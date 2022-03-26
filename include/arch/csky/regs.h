/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ARCH_CSKY_REGS_H_
#define _ARCH_CSKY_REGS_H_

#include <bits.h>

/* PSR CR0 */
#define PSR_S               BIT(31)
#define PSR_CPUID           BIT_RANGE(27, 24)
#define PSR_VEC             BIT_RANGE(22, 16)
#define PSR_TM              BIT_RANGE(15, 14)
#define PSR_TP              BIT(13)
#define PSR_TE              BIT(12)
#define PSR_MM              BIT(9)
#define PSR_EE              BIT(8)
#define PSR_IC              BIT(7)
#define PSR_IE              BIT(6)
#define PSR_FE              BIT(4)
#define PSR_AF              BIT(1)
#define PSR_C               BIT(0)

/* CPUIDRR CR13 */
#define CPUIDRR_FAMILY      BIT_RANGE(31, 28)
#define CPUIDRR_MODEL       BIT_RANGE(27, 24)
#define CPUIDRR_IC          BIT_RANGE(23, 20)
#define CPUIDRR_DC          BIT_RANGE(19, 16)
#define CPUIDRR_FOUND       BIT_RANGE(15, 12)
#define CPUIDRR_ISPM        BIT(11)
#define CPUIDRR_DSPM        BIT(10)
#define CPUIDRR_PROC        BIT_RANGE(9, 8)
#define CPUIDRR_REV         BIT_RANGE(7, 4)
#define CPUIDRR_VER         BIT_RANGE(3, 0)

/* CFR CR17 */
#define CFR_LICF            BIT(31)
#define CFR_ITS             BIT(7)
#define CFR_OMS             BIT(6)
#define CFR_CLR             BIT(5)
#define CFR_INV             BIT(4)
#define CFR_SELD            BIT(1)
#define CFR_SELI            BIT(0)

/* CCR CR18 */
#define CCR_CLK             BIT_RANGE(9, 8)
#define CCR_BE              BIT(7)
#define CCR_Z               BIT(6)
#define CCR_RS              BIT(5)
#define CCR_WB              BIT(4)
#define CCR_DE              BIT(3)
#define CCR_IE              BIT(2)
#define CCR_BP              BIT(0)

/* CP15 CR0 */
#define MIR_P               BIT(31)
#define MIR_TF              BIT(30)
#define MIR_INDEX           BIT_RANGE(9, 0)

/* CP15 CR4 */
#define MEH_VPN             BIT_RANGE(31, 12)
#define MEH_ASID            BIT_RANGE(7, 0)

/* CP15 CR8 */
#define MCIR_TLBP           BIT(31)
#define MCIR_TLBR           BIT(30)
#define MCIR_TLBWI          BIT(29)
#define MCIR_TLBWR          BIT(28)
#define MCIR_TLBINV         BIT(27)
#define MCIR_TLBINVA        BIT(26)
#define MCIR_TLBINVD        BIT(25)
#define MCIR_ASID           BIT(0)

/* CP15 CR29 */
#define MPGD_PBA            BIT_RANGE(31, 12)
#define MPGD_HRE            BIT(0)

/* CP15 CR30/31 */
#define MSA_BA              BIT_RANGE(31, 29)
#define MSA_CACHE           BIT(3)
#define MSA_DIRTY           BIT(2)
#define MSA_VAILD           BIT(1)

#endif  /* _ARCH_CSKY_REGS_H_ */
