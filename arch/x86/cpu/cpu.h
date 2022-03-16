/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ARCH_X86_CPU_H_
#define _ARCH_X86_CPU_H_

struct cpu_type {
    const char *vendor;
    const char *ident;
    unsigned int type;

    void (*early_init)()

};

#define DEFINE_CPU_TYPE(name) \
    static const __section(".x86_cpu_types") \
    struct cpu_type name

#endif  /* _ARCH_X86_CPU_H_ */
