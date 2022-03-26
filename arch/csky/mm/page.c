/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <memory.h>
#include <asm/pgtable.h>
#include <asm/cache.h>
#include <asm/tlb.h>

struct pgd page_dir[PTRS_PER_PGD] __aligned(PAGE_SIZE);
struct pte invalid_pte_table[PTRS_PER_PTE] __aligned(PAGE_SIZE);

/*
 *  Kernel page table
 *
 *   Addr | 0               2G      3G
 *        | ################|++++++++|########
 *  Space |      User       ^ Kernel ^ Himem
 *
 *  User:   This part is blank
 *  Kernel: Use fixed huge page mapping
 *  Himem:  Use statically assigned pt_himem
 *
 */

void __init arch_page_setup(void)
{
    /* Mapping kernel space directly */
    mmu_msa0_set(CONFIG_RAM_BASE | MSA_CACHE | MSA_DIRTY | MSA_VAILD);
    mmu_msa1_set(CONFIG_RAM_BASE | MSA_DIRTY | MSA_VAILD);
}
