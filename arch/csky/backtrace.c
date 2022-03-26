/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kernel.h>
#include <printk.h>
#include <asm/backtrace.h>
#include <asm/page.h>
#include <asm/regs.h>

struct stackframe {
    uint32_t fp;
    uint32_t pc;
};

void backtrace_regs(struct regs *regs)
{
    printk("General Register:\r\n");
    printk("  PC: 0x%08x PSR: 0x%08x\r\n", regs->pc, regs->psr);
    printk("  A0: 0x%08x A1: 0x%08x A2: 0x%08x A3: 0x%08x\r\n",
            regs->a0, regs->a1, regs->a2, regs->a3);
    printk("  A4: 0x%08x A5: 0x%08x L0: 0x%08x L1: 0x%08x\r\n",
            regs->a4, regs->a5, regs->l0, regs->l1);
    printk("  L2: 0x%08x L3: 0x%08x L4: 0x%08x L5: 0x%08x\r\n",
            regs->l2, regs->l3, regs->l4, regs->l5);
    printk("  R1: 0x%08x SP: 0x%08x LR: 0x%08x GB: 0x%08x\r\n",
            regs->r1, regs->sp, regs->lr, regs->gb);

    printk("Control Register:\r\n");
    printk("  VBR: 0x%08x SS0: 0x%08x SS1: 0x%08x SS2: 0x%08x\r\n",
            mfcr("cr1"), mfcr("cr6"), mfcr("cr7"), mfcr("cr8"));
    printk("  SS3: 0x%08x SS4: 0x%08x GCR: 0x%08x GSR: 0x%08x\r\n",
            mfcr("cr9"), mfcr("cr10"), mfcr("cr11"), mfcr("cr12"));
    printk("  IDR: 0x%08x DCS: 0x%08x CWR: 0x%08x CFR: 0x%08x\r\n",
            mfcr("cr13"), mfcr("cr14"), mfcr("cr15"), mfcr("cr17"));
    printk("  CCR: 0x%08x CPR: 0x%08x PCR: 0x%08x PSE: 0x%08x\r\n",
            mfcr("cr18"), mfcr("cr19"), mfcr("cr20"), mfcr("cr21"));

    printk("MMU Register:\r\n");
    printk("  MIR: 0x%08x MRR: 0x%08x ME0: 0x%08x ME1: 0x%08x\r\n",
            cprcr("cpcr0"), cprcr("cpcr1"), cprcr("cpcr2"), cprcr("cpcr3"));
    printk("  MEH: 0x%08x MCR: 0x%08x MPR: 0x%08x MWR: 0x%08x\r\n",
            cprcr("cpcr4"), cprcr("cpcr5"), cprcr("cpcr6"), cprcr("cpcr7"));
    printk("  MCIR: 0x%08x ISPM: 0x%08x DSPM: 0x%08x\r\n",
            cprcr("cpcr8"), cprcr("cpcr9"), cprcr("cpcr10"));
    printk("  DMAE: 0x%08x DMAC: 0x%08x DMAS: 0x%08x\r\n",
            cprcr("cpcr11"), cprcr("cpcr12"), cprcr("cpcr13"));
    printk("  DMAO: 0x%08x DMAI: 0x%08x DMAL: 0x%08x\r\n",
            cprcr("cpcr14"), cprcr("cpcr15"), cprcr("cpcr16"));
    printk("  MPGD: 0x%08x SEG0: 0x%08x SEG1: 0x%08x\r\n",
            cprcr("cpcr29"), cprcr("cpcr30"), cprcr("cpcr31"));
}

void backtrace_stack(size_t *sp)
{
    printk("stack:\n");
    for (int i = 0; i < 8; i++) {
        printk(" [%p]: ", sp);
        for (int i = 0; i < 4; i++)
            printk("%08lx ", *sp++);
        printk("\n");
    }
}

void backtrace_call(struct regs *regs)
{
    struct stackframe *frame;
    size_t sp, fp, pc;
    size_t min, max;

    sp = regs->sp;
    fp = regs->l0;
    pc = regs->pc;

    printk("backtrace:\n");
    for (;;) {
        min = sp;
        max = align_high(sp, THREAD_SIZE);

        printk(" [0x%08lx]: \n", pc);
        if (unlikely(fp < min || max < fp || fp & 0x3))
            break;

        frame = (struct stackframe *)fp;
        sp = fp;
        fp = frame->fp;
        pc = frame->pc;
    }
}

void backtrace(struct regs *regs)
{
    backtrace_regs(regs);
    backtrace_stack((size_t *)regs->sp);
    backtrace_call(regs);
}
