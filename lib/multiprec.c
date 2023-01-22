/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <multiprec.h>
#include <kmalloc.h>
#include <export.h>

static struct kcache *multiprec_kcache;

struct multiprec_num *multiprec_alloc(unsigned int integer, unsigned int decimal)
{
    struct multiprec_num *mpnum;

    mpnum = kcache_zalloc(multiprec_kcache, GFP_KERNEL);
    if (unlikely(!mpnum))
        return NULL;

    mpnum->integer = integer;
    mpnum->decimal = decimal;

    mpnum->value = kzalloc(integer + decimal, GFP_KERNEL);
    if (unlikely(!mpnum->value)) {
        kcache_free(multiprec_kcache, mpnum);
        return NULL;
    }

    return mpnum;
}

struct multiprec_num *multiprec_free()
{


}

struct multiprec_num *multiprec_intnum(int value)
{
    struct multiprec_num *mpnum;
    unsigned int count, index;
    char buffer[30];
    bool neg;

    if ((neg = value < 0))
        value = -value;








    mpnum = kmalloc(sizeof(*mpnum));

}
EXPORT_SYMBOL(multiprec_intnum);

int multiprec_numint(const struct multiprec_num *val)
{


}
EXPORT_SYMBOL(multiprec_numint);

struct multiprec_num *multiprec_strnum(const char *str)
{


}
EXPORT_SYMBOL(multiprec_strnum);

const char *multiprec_numstr(const struct multiprec_num *val)
{


}
EXPORT_SYMBOL(multiprec_numstr);
