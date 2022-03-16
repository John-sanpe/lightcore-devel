/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "x86-intel"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include "cpu.h"


DEFINE_CPU_TYPE(intel) = {
    .vendor = "Intel",
    .ident = { "GenuineIntel" },

};
