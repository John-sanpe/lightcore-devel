#ifndef _TSEARCH_H_
#define _TSEARCH_H_

#include <kernel.h>
#include <poison.h>

enum tsvisit;
#define TSDEPTH 32

typedef long (*tsnode_cmp)(const struct tsnode *tsa, const struct tsnode *tsb);
typedef long (*tsnode_find)(const struct tsnode *tsnode, const void *key);
typedef long (*tsnode_action)(const struct tsnode *ts, unsigned int depth, enum tsvisit visit);

enum tsvisit {
    TSVISIT_LEAF        = 0,
    TSVISIT_PERORDER    = 1,
    TSVISIT_POSTORDER   = 2,
    TSVISIT_ENDORDER    = 3,
};

struct tsnode {
    union {
        struct {
            struct tsnode *left;
            struct tsnode *right;
        };
        struct tsnode *child[2];
    };
    unsigned int height;
};

struct tsroot {
    struct tsnode *node;
};

extern struct tsnode *tsearch(struct tsroot *root, struct tsnode *node, tsnode_cmp cmp);
extern struct tsnode *tdelete(struct tsroot *root, const void *key, tsnode_find find);
extern struct tsnode *tfind(const struct tsroot *root, const void *key, tsnode_find find);
extern void twalk(const struct tsroot *root, tsnode_action action);

#endif  /* _TSEARCH_H_ */
