/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

struct lex_word {
    const char *word;

};

const struct lex_word keywords[] = {
    {"BEGIN",     ,},
    {"END",       ,},
    {"NF",        ,},
    {"atan2",     ,},
    {"break",     ,},
    {"close",     ,},
    {"continue"   ,},
    {"cos",       ,},
    {"delete",    ,},
    {"do",        ,},
    {"else",      ,},
    {"exit",      ,},
    {"exp",       ,},
    {"fflush",    ,},
    {"for",       ,},
    {"func",      ,},
    {"function",  ,},
    {"getline",   ,},
    {"gsub",      ,},
    {"if",        ,},
    {"in",        ,},
    {"index",     ,},
    {"int",       ,},
    {"length",    ,},
    {"log",       ,},
    {"match",     ,},
    {"next",      ,},
    {"nextfile"   ,},
    {"print",     ,},
    {"printf",    ,},
    {"rand",      ,},
    {"return",    ,},
    {"sin",       ,},
    {"split",     ,},
    {"sprintf",   ,},
    {"sqrt",      ,},
    {"srand",     ,},
    {"sub",       ,},
    {"substr",    ,},
    {"system",    ,},
    {"tolower",   ,},
    {"toupper",   ,},
    {"while",     ,},
};

static int awk_lex()
{



}

