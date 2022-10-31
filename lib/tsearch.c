/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <tsearch.h>
#include <export.h>

static long tsearch_balance(struct tsnode **nodep)
{
    #define hight(node) (node ? node->height : 0)
    struct tsnode *walk = *nodep;
    unsigned int ldep, rdep;

    ldep = hight(walk->left);
    rdep = hight(walk->right);

    if (ldep - rdep < 2) {
        unsigned int old = walk->height;
        walk->height = ldep < rdep ? rdep + 1 : ldep + 1;
        return walk->height - old;
    } else {
        struct tsnode *child1, *child2;
        unsigned int height1, height2;
        bool dir = ldep < rdep;

        child1 = walk->child[dir];
        child2 = walk->child[!dir];

        height1 = walk->height;
        height2 = hight(child2);

        if (height2 > hight(walk->child[dir])) {
            walk->child[dir] = child2->child[!dir];
            child1->child[!dir] = child2->child[dir];
            child2->child[!dir] = walk;
            child2->child[dir] = child1;
            walk->height = child1->height = height2;
            child2->height = height2 + 1;
        } else {
            walk->child[dir] = child2;
            child1->child[!dir] = walk;
            walk->height = height2 + 1;
            child1->height = height2 + 2;
            child2 = child1;
        }

        *nodep = child2;
        return child2->height - height1;
    }
}

struct tsnode *tsearch(struct tsroot *root, struct tsnode *node, tsnode_cmp cmp)
{
    struct tsnode *walk = root->node;
    struct tsnode **stack[TSDEPTH];
    unsigned int index = 0;
    long ret;

    stack[index++] = &root->node;
    node->left = node->right = NULL;
    node->height = 1;

    while (walk) {
        ret = cmp(node, walk);
        if (ret == 0)
            return walk;
        stack[index++] = &walk->child[ret > 0];
        walk = walk->child[ret > 0];
    }

    *stack[--index] = node;
    while (index-- && tsearch_balance(stack[index]));

    return node;
}
EXPORT_SYMBOL(tsearch);

struct tsnode *tdelete(struct tsroot *root, const void *key, tsnode_find find)
{
    struct tsnode *node = root->node;
    struct tsnode *child, *removed;
    struct tsnode **stack[TSDEPTH];
    unsigned int index = 0;
    long ret;

    stack[index++] = &root->node;

    while (node) {
        ret = find(node, key);
        if (ret == LONG_MIN)
            return NULL;
        stack[index++] = &node->child[ret > 0];
        node = node->child[ret > 0];
    }

    if (!node->left)
        child = node->right;
    else {
        removed = node;
        stack[index++] = &node->left;
        node = node->left;
        while (node->right) {
            stack[index++] = &node->right;
            node = node->right;
        }
        removed->key = node->key;
        child = node->left;
    }

    *stack[--index] = child;
    while (index-- && tsearch_balance(stack[index]));

    return node;
}
EXPORT_SYMBOL(tdelete);

struct tsnode *tfind(const struct tsroot *root, const void *key, tsnode_find find)
{
    struct tsnode *node = root->node;
    long ret;

    while (node) {
        ret = cmp(node, key);
        if (ret == LONG_MIN)
            return NULL;
        else if (ret == 0)
            return node;
        node = node->child[ret > 0];
    }

    return node;
}
EXPORT_SYMBOL(tfind);

static void walk(const struct tsnode *node, tsnode_action action, unsigned int depth)
{
    if (!node || depth > TSDEPTH)
        return;
    if (node->height == 1)
        action(node, depth, TSVISIT_LEAF);
    else {
        action(node, depth, TSVISIT_PERORDER);
        walk(node->left, action, depth + 1);
        action(node, depth, TSVISIT_POSTORDER);
        walk(node->right, action, depth + 1);
        action(node, depth, TSVISIT_ENDORDER);
    }
}

void twalk(const struct tsroot *root, tsnode_action action)
{
    walk(root->node, action, 0);
}
EXPORT_SYMBOL(twalk);
