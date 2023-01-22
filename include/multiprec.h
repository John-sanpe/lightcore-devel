/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _MULTIPREC_H_
#define _MULTIPREC_H_

#include <types.h>

enum multiprec_sign {
    MULTIPREC_PLUS,
    MULTIPREC_ZERO,
    MULTIPREC_MINUS,
};

struct multiprec_num {
    enum multiprec_sign sign;
    unsigned int integer;
    unsigned int decimal;
    char *value;
};

extern struct multiprec_num *multiprec_intnum(int value);
extern int multiprec_numint(const struct multiprec_num *val);
extern struct multiprec_num *multiprec_strnum(const char *str);
extern const char *multiprec_numstr(const struct multiprec_num *val);

extern enum multiprec_sign multiprec_cmp(const struct multiprec_num *vala, const struct multiprec_num *valb);
extern struct multiprec_num *multiprec_add(const struct multiprec_num *vala, const struct multiprec_num *valb);
extern struct multiprec_num *multiprec_sub(const struct multiprec_num *vala, const struct multiprec_num *valb);
extern struct multiprec_num *multiprec_multiply(const struct multiprec_num *vala, const struct multiprec_num *valb);
extern struct multiprec_num *multiprec_divide(const struct multiprec_num *vala, const struct multiprec_num *valb);
extern struct multiprec_num *multiprec_modulo(const struct multiprec_num *vala, const struct multiprec_num *valb);

#endif  /* _MULTIPREC_H_ */
