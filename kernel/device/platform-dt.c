/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define pr_fmt(fmt)	"pdt: " fmt

#include <kmalloc.h>
#include <initcall.h>
#include <driver/platform.h>
#include <driver/dt.h>
#include <printk.h>

static const struct dt_device_id dt_bus_table[] = {
    { .compatible = "simple-bus", .data = (void *)RESOURCE_MMIO },
    { .compatible = "amba-bus", .data = (void *)RESOURCE_MMIO },
    { .compatible = "isa-bus", .data = (void *)RESOURCE_PMIO },
    { }, /* NULL */
};

static const struct dt_device_id dt_skipped_table[] = {
    { .compatible = "operating-points-v2", },
    { }, /* NULL */
};

static inline const struct dt_device_id *
dt_device_match(const struct dt_device_id *ids, struct dt_node *node)
{
    while ((ids->name && ids->name[0]) || (ids->type && ids->type[0]) ||
          (ids->compatible && ids->compatible[0])) {
        if(dt_match(ids, node))
            return ids;
        ++ids;
    }
    return NULL;
}

const struct dt_device_id *
platform_dt_match(struct platform_driver *pdrv, struct platform_device *pdev)
{
    if (!pdrv->dt_table || !pdev->dt_node)
        return NULL;
    return dt_device_match(pdrv->dt_table, pdev->dt_node);
}

static __always_inline struct platform_device *dt_alloc_node(struct dt_node *node)
{
    struct platform_device *pdev;

    pdev = platform_device_alloc(NULL, 0);
    if (!pdev)
        return NULL;

    pdev->dt_node = node;
	return pdev;
}

static inline state dt_populate_resource(struct platform_device *pdev, int type)
{
    struct resource *res;
    int res_nr, count;

    res_nr = dt_address_nr(pdev->dt_node);

    res = kzalloc(sizeof(*pdev->resource) * res_nr, GFP_KERNEL);
    if (!res)
        return -ENOMEM;

    pdev->resource = res;
    pdev->resources_nr = res_nr;

    for (count = 0; count < res_nr; ++count, ++res) {
        resource_size_t size;

        if(dt_address(pdev->dt_node, count, &res->start, &size))
            break;

        res->size = size;
        res->type = type;
        dt_attribute_read_string_index(pdev->dt_node, "reg-names", count, &res->name);
    }

    return -ENOERR;
}

static state platform_dt_setup_node(struct dt_node *node, const struct dt_device_id *id)
{
	struct platform_device *pdev;

    pdev = dt_alloc_node(node);
    if (!pdev)
        return -ENOMEM;

    pr_debug("New device: %s\n", node->path);
    pdev->dev.name = node->path;
    dt_populate_resource(pdev, (int)id->data);

    return device_register(&pdev->dev);
}

state platform_dt_setup_bus(struct dt_node *bus, const struct dt_device_id *parent,
                            const struct dt_device_id *matches)
{
    const struct dt_device_id *bus_id;
    struct dt_node *node;
    state ret;

    if (!dt_attribute_get(bus, "compatible", NULL))
        return -ENOERR;

    if (unlikely(dt_match(dt_skipped_table, bus)))
        return -ENOERR;

    bus_id = dt_device_match(matches, bus);
    if (!bus_id && !parent)
        return -ENOERR;
    else if (!bus_id)
        return platform_dt_setup_node(bus, parent);

    dt_for_each_child(node, bus) {
        ret = platform_dt_setup_bus(node, bus_id, matches);
        if (ret)
            return ret;
    }

    return -ENOERR;
}

void __init platform_dt_init(void)
{
    struct dt_node *bus;
    state ret;

    if (!dt_root)
        return;

    pr_debug("populate at: %p\n", dt_root);

    dt_for_each_child(bus, dt_root) {
        ret = platform_dt_setup_bus(bus, NULL, dt_bus_table);
        if (ret)
            break;
    }
}
