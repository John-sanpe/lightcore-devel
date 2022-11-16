/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define pr_fmt(fmt) "ioremap: " fmt

#include <ioremap.h>
#include <memory.h>
#include <kmalloc.h>
#include <mm/vmem.h>
#include <mm/vmap.h>
#include <kernel.h>
#include <rbtree.h>
#include <refcount.h>
#include <export.h>
#include <printk.h>
#include <panic.h>

static SPIN_LOCK(ioremap_lock);
static RB_ROOT(ioremap_root);
static struct kcache *ioremap_cache;

struct ioremap_node {
    struct rb_node rb;
    struct vmem_area vm_area;
    phys_addr_t addr;
    gvm_t flags;
    unsigned int count;
};

#define rb_to_ioremap(io) \
    rb_entry(io, struct ioremap_node, rb)

#define rb_to_ioremap_safe(io) \
    rb_entry_safe(io, struct ioremap_node, rb)

#define vm_to_ioremap(io) \
    container_of(io, struct ioremap_node, vm_area)

struct ioremap_find {
    phys_addr_t addr;
    size_t size;
    gvm_t flags;
};

static long ioremap_rb_cmp(const struct rb_node *rba, const struct rb_node *rbb)
{
    struct ioremap_node *node_a = rb_to_ioremap(rba);
    struct ioremap_node *node_b = rb_to_ioremap(rbb);

    if (node_a->flags != node_b->flags)
        return node_a->flags < node_b->flags ? -1 : 1;

    if (node_a->addr != node_b->addr)
        return node_a->addr < node_b->addr ? -1 : 1;

    if (node_a->vm_area.size != node_b->vm_area.size)
        return node_a->vm_area.size < node_b->vm_area.size ? -1 : 1;

    BUG();
    return 0;
}

static long ioremap_rb_find(const struct rb_node *rb, const void *key)
{
    struct ioremap_node *node = rb_to_ioremap(rb);
    const struct ioremap_find *cmp = key;
    phys_addr_t node_end, cmp_end;

    node_end = node->addr + node->vm_area.size - 1;
    cmp_end = cmp->addr + cmp->size - 1;

    if (node->flags == cmp->flags &&
        node->addr <= cmp->addr && cmp_end <= node_end)
        return 0;

    if (node->flags != cmp->flags)
        return node->flags > cmp->flags ? -1 : 1;

    if (node->addr != cmp->addr)
        return node->addr > cmp->addr ? -1 : 1;

    if (node->vm_area.size != cmp->size)
        return node->vm_area.size > cmp->size ? -1 : 1;

    return LONG_MIN;
}

static struct ioremap_node *phys_addr_find(phys_addr_t start, size_t size, gvm_t flags)
{
    struct rb_node *rb;
    rb = rb_find(&ioremap_root, &(struct ioremap_find){start, size, flags}, ioremap_rb_find);
    return rb_to_ioremap_safe(rb);
}

static struct ioremap_node *ioremap_alloc(phys_addr_t phys, size_t size, gvm_t flags)
{
    struct ioremap_node *node;
    state ret;

    node = kcache_zalloc(ioremap_cache, GFP_KERNEL);
    if (unlikely(!node))
        return NULL;

    ret = vmem_area_alloc(&node->vm_area, size);
    if (unlikely(ret))
        goto error_free;

    ret = vmap_range(&init_mm, phys, node->vm_area.addr, node->vm_area.size, flags, PGDIR_SHIFT);
    if (unlikely(ret))
        goto error_vmfree;

    node->addr = phys;
    node->flags = flags;
    rb_insert(&ioremap_root, &node->rb, ioremap_rb_cmp);

    return node;

error_vmfree:
    vmem_area_free(&node->vm_area);
error_free:
    kfree(node);
    return NULL;
}

void *ioremap_node(phys_addr_t phys, size_t size, gvm_t flags)
{
    struct ioremap_node *node;
    phys_addr_t offset, last_addr;

    /* Don't allow wraparound or zero size */
    last_addr = phys + size - 1;
    if (unlikely(!size || last_addr < phys))
        return NULL;

    /* page-align mappings */
    offset = phys & ~PAGE_MASK;
    phys &= PAGE_MASK;
    size = page_align(last_addr + 1) - phys;

    spin_lock(&ioremap_lock);

    node = phys_addr_find(phys, size, flags);
    if (node)
        goto done;

    node = ioremap_alloc(phys, size, flags);
    if (unlikely(!node))
        goto error;

done:
    node->count++;
    spin_unlock(&ioremap_lock);
    return (void *)(node->vm_area.addr + offset);

error:
    spin_unlock(&ioremap_lock);
    return NULL;
}
EXPORT_SYMBOL(ioremap_node);

void *ioremap(phys_addr_t phys, size_t size)
{
#ifdef CONFIG_ARCH_HAS_IOMAP
    if (phys >= CONFIG_RAM_BASE && phys < CONFIG_HIGHMEM_OFFSET)
        return pa_to_io(phys);
#endif
    return ioremap_node(phys, size, GVM_NOCACHE);
}
EXPORT_SYMBOL(ioremap);

void *ioremap_uc(phys_addr_t phys, size_t size)
{
    if (phys >= CONFIG_RAM_BASE && phys < CONFIG_HIGHMEM_OFFSET)
        return pa_to_va(phys);

    return ioremap_node(phys, size, 0);
}
EXPORT_SYMBOL(ioremap_uc);

void *ioremap_wc(phys_addr_t phys, size_t size)
{
    return ioremap_node(phys, size, GVM_WCOMBINED);
}
EXPORT_SYMBOL(ioremap_wc);

void *ioremap_wt(phys_addr_t phys, size_t size)
{
    return ioremap_node(phys, size, GVM_WTHROUGH);
}
EXPORT_SYMBOL(ioremap_wt);

void iounmap(void *block)
{
    size_t addr = (size_t)block;
    struct ioremap_node *node;
    struct vmem_area *vm;

    if (addr < CONFIG_HIGHMAP_OFFSET)
        return;

    spin_lock(&ioremap_lock);
    vm = vmem_area_find(addr);
    if (!vm) {
        pr_crit("abort iounmap %p\n", block);
        goto finish;
    }

    node = vm_to_ioremap(vm);
    if (--node->count)
        goto finish;

    rb_delete(&ioremap_root, &node->rb);
    spin_unlock(&ioremap_lock);

    vmem_area_free(vm);
    vunmap_range(&init_mm, vm->addr, vm->size);
    kcache_free(ioremap_cache, node);

    return;

finish:
    spin_unlock(&ioremap_lock);
}
EXPORT_SYMBOL(iounmap);

void __init ioremap_init(void)
{
    ioremap_cache = kcache_create("ioremap",
        sizeof(struct ioremap_node), KCACHE_PANIC);
}
