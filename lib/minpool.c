/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <minpool.h>
#include <export.h>

struct minpool_node {
    struct list_head block;
    struct list_head free;
    size_t usize;
    char data[0];
};

static __always_inline bool
gnode_get_used(struct minpool_node *node)
{
    return node->usize & BIT(0);
}

static __always_inline size_t
gnode_get_size(struct minpool_node *node)
{
    return node->usize & BIT_HIGH_MASK(0);
}

static __always_inline void
gnode_set_used(struct minpool_node *node, bool used)
{
    node->usize &= ~BIT(0);
    node->usize |= used;
}

static __always_inline void
gnode_set_size(struct minpool_node *node, size_t size)
{
    node->usize &= ~BIT_HIGH_MASK(0);
    node->usize |= size & BIT_HIGH_MASK(0);
}

static __always_inline void
gnode_set(struct minpool_node *node, size_t size, bool used)
{
    node->usize = (size & BIT_HIGH_MASK(0)) | used;
}

/**
 * minpool_align - Minimum mempool align allocation.
 * @head: Minimum mempool to operate.
 * @size: Size to allocation.
 * @size: Align to allocation.
 */
void *minpool_align(struct minpool_head *head, size_t size, size_t align)
{
    struct minpool_node *node, *free = NULL;
    size_t fsize, delta;

    align = max(align_high(align, MINPOOL_ALIGN), MINPOOL_ALIGN);
    size = align_high(size, align);

    if (unlikely(size > head->avail))
        return NULL;

    /* Get the free memory block */
    list_for_each_entry(node, &head->free_list, free) {
        delta = align_high((uintptr_t)node->data, align) - (uintptr_t)node;
        fsize = gnode_get_size(node);
        if (delta + size <= fsize) {
            free = node;
            break;
        }
    }

    if (unlikely(!free))
        return NULL;

    if (delta) {
        struct minpool_node *prev;

        prev = list_prev_entry_or_null(node, &head->block_list, block);
        if ()

        /* Prevent self coverage */
        list_del(&node->block);
        list_del_init(&node->free);

        /* Setup the used node */
        node = align_ptr_high((void *)node->data, align) - sizeof(*node);
        list_del(&node->block);

        /* Setup the prev free block */
        if (delta >= MINPOOL_ALIGN)

        else {


        }
    }

    gnode_set(node, fsize - delta, true);

    /* Setup the next free block */
    fsize =
    if (fsize - size > sizeof(*free) + MINPOOL_ALIGN) {
        free = (void *)(node->data + size);
        gnode_set_size(free, fsize - size - sizeof(*free));
        gnode_set_used(free, false);

        list_add(&head->free_list, &free->free);
        list_add(&node->block, &free->block);

        gnode_set_size(node, size);
        head->avail -= sizeof(*free);
        fsize = size;
    }

    /* Adjust heap available size */
    head->avail -= fsize;
    return node->data;
}
EXPORT_SYMBOL(minpool_align);

/**
 * minpool_free - Minimum mempool release.
 * @head: Minimum mempool to operate.
 * @block: Block address to free.
 */
void minpool_free(struct minpool_head *head, void *block)
{
    struct minpool_node *other, *node;

    if (unlikely(!block))
        return;

    /* Check whether it's a legal node */
    node = block - offsetof(struct minpool_node, data);
    if (unlikely(!list_check_empty(&node->free)))
        return;

    /* Set node freed */
    head->avail += gnode_get_size(node);
    gnode_set_used(node, false);
    list_add(&head->free_list, &node->free);

    /* Merge previous node */
    other = list_prev_entry(node, block);
    if (!list_entry_check_head(other, &head->block_list, block) && !gnode_get_used(other)) {
        gnode_set_size(other, sizeof(*node) + gnode_get_size(node));
        list_del(&node->block);
        list_del(&node->free);
        head->avail += sizeof(*node);
        node = other;
    }

    /* Merge next node */
    other = list_next_entry(node, block);
    if (!list_entry_check_head(other, &head->block_list, block) && !gnode_get_used(other)) {
        gnode_set_size(node, sizeof(*other) + gnode_get_size(node));
        list_del(&other->block);
        list_del(&other->free);
        head->avail += sizeof(*other);
    }
}
EXPORT_SYMBOL(minpool_free);

/**
 * minpool_setup - Minimum mempool setup.
 * @head: Minimum mempool to operate.
 * @array: Mempool array address.
 * @size: Mempool array size.
 */
void minpool_setup(struct minpool_head *head, void *array, size_t size)
{
    struct minpool_node *node = array;

    gnode_set_used(node, false);
    gnode_set_size(node, size - sizeof(*node));
    head->avail = size - sizeof(*node);

    list_head_init(&head->block_list);
    list_head_init(&head->free_list);
    list_add(&head->block_list, &node->block);
    list_add(&head->free_list, &node->free);
}
EXPORT_SYMBOL(minpool_setup);
