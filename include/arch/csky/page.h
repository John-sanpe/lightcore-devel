/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#ifndef _ARCH_CSKY_PAGE_H_
#define _ARCH_CSKY_PAGE_H_

#include <types.h>

#define PAGE_PRESENT        BIT(0)  /* Is Present */
#define PAGE_READ           BIT(1)  /* Readable */
#define PAGE_WRITE          BIT(2)  /* Writeable */
#define PAGE_ACCESSED       BIT(3)  /*  */
#define PAGE_MODIFIED       BIT(4)  /*  */
#define PAGE_GLOBAL         BIT(5)  /* Global Tlb Entry */
#define PAGE_VALID          BIT(6)  /*  */
#define PAGE_DIRTY          BIT(7)  /* Was Written To (Raised By Cpu) */

#ifndef __ASSEMBLY__

typedef unsigned long pmdval_t;
typedef unsigned long pteval_t;

struct pgd {
    union {
        struct {
            uint32_t res:12;
            uint32_t addr:20;
        };
        uint32_t val;
    };
};

struct pte {
    union {
        struct {
            /* Software Implemented */
            uint32_t p:1;       /* Page present (0:non-existent 1:In memory) */
            uint32_t r:1;       /* Read if 0, reads may not be allowed */
            uint32_t w:1;       /* Write if 0, writes may not be allowed */
            uint32_t a:1;       /* Accessed indicates whether software has accessed */
            uint32_t m:1;       /* Modified indicates whether software has modified */

            /* Hardware Implemented */
            uint32_t g:1;       /* Global */
            uint32_t v:1;       /* Valid */
            uint32_t d:1;       /* Dirty indicates whether software has written */
            uint32_t cache:3;   /*  */
            uint32_t addr:20;   /* Physical address of the 4-KByte page referenced by this entry */
        };
        uint32_t val;
    };
} __packed;

#define PAGE_CACHE      0x03
#define PAGE_UNCACHE    0x02

#endif  /* __ASSEMBLY__ */
#endif  /* _ARCH_ARM_PAGE_H_ */
