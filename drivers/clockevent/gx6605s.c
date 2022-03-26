/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "gx6605s-timer"
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include <initcall.h>
#include <irq.h>
#include <timer.h>
#include <ioops.h>
#include <driver/irqchip.h>
#include <driver/platform.h>
#include <driver/clockevent.h>
#include <driver/clocksource.h>
#include <driver/clocksource/gx6605s.h>

struct gx6605s_device {
    struct clockevent_device clkevt;
    struct clocksource_device clksrc;
    struct irqchip_channel *irqchip;
    bool oneshot;
    void *mmio;
};

#define clkevt_to_gdev(cdev) \
    container_of(cdev, struct gx6605s_device, clkevt)

#define clksrc_to_gdev(cdev) \
    container_of(cdev, struct gx6605s_device, clksrc)

static __always_inline uint32_t
gx6605s_read(struct gx6605s_device *gdev, uint16_t reg)
{
    return readl(gdev->mmio + reg);
}

static __always_inline void
gx6605s_write(struct gx6605s_device *gdev, int reg, uint32_t val)
{
    writel(gdev->mmio + reg, val);
}

static __always_inline void gx6605s_clkevt_start(struct gx6605s_device *gdev)
{
    gx6605s_write(gdev, GX6605S_TIM_CONFIG, GX6605S_TIM_CONFIG_IRQ_EN | GX6605S_TIM_CONFIG_EN);
    gx6605s_write(gdev, GX6605S_TIM_CONTRL, GX6605S_TIM_CONTRL_START);
}

static __always_inline void gx6605s_clkevt_reset(struct gx6605s_device *gdev)
{
    gx6605s_write(gdev, GX6605S_TIM_CONTRL, GX6605S_TIM_CONTRL_RST);
    gx6605s_write(gdev, GX6605S_TIM_CONTRL, 0);
}

static __always_inline void gx6605s_clkevt_clear(struct gx6605s_device *gdev)
{
    if (gdev->oneshot)
        gx6605s_write(gdev, GX6605S_TIM_RELOAD, 0);
    gx6605s_write(gdev, GX6605S_TIM_STATUS, GX6605S_TIM_STATUS_CLR);
}

irqreturn_t gx6605s_clkevt_handle(irqnr_t vector, void *data)
{
    struct gx6605s_device *gdev = data;

    gx6605s_clkevt_clear(gdev);
    gdev->clkevt.event_handle(&gdev->clkevt);

    return IRQ_RET_HANDLED;
}

static state gx6605s_clkevt_next_event(struct clockevent_device *cdev, uint64_t delta)
{

    return -ENOERR;
}

static state gx6605s_clkevt_state_oneshot(struct clockevent_device *cdev)
{
    struct gx6605s_device *gdev = clkevt_to_gdev(cdev);

    gdev->oneshot = true;

    gx6605s_clkevt_reset(gdev);
    gx6605s_clkevt_start(gdev);

    return -ENOERR;
}

static state gx6605s_clkevt_state_periodic(struct clockevent_device *cdev, uint64_t delta)
{
    struct gx6605s_device *gdev = clkevt_to_gdev(cdev);
    uint32_t val;

    gdev->oneshot = false;

    gx6605s_clkevt_reset(gdev);
    val = UINT32_MAX - (1000000 / CONFIG_SYSTICK_FREQ) + 1;
    gx6605s_write(gdev, GX6605S_TIM_RELOAD, val);
    gx6605s_clkevt_start(gdev);

    return -ENOERR;
}

static state gx6605s_clkevt_state_shutdown(struct clockevent_device *cdev)
{
    struct gx6605s_device *gdev = clkevt_to_gdev(cdev);

    gx6605s_write(gdev, GX6605S_TIM_CONTRL, 0);
    gx6605s_write(gdev, GX6605S_TIM_CONFIG, 0);

    return -ENOERR;
}

static struct clockevent_ops gx6605s_clkevt_ops = {
    .next_event = gx6605s_clkevt_next_event,
    .state_oneshot = gx6605s_clkevt_state_oneshot,
    .state_periodic = gx6605s_clkevt_state_periodic,
    .state_shutdown = gx6605s_clkevt_state_shutdown,
};

#ifdef CONFIG_CLKSRC_GX6605S

static struct clocksource_ops gx6605s_clksrc_ops = {
    .read = gx6605s_clksrc_read,
};
#endif

static void gx6605s_clkevt_hwinit(struct gx6605s_device *gdev)
{
    uint32_t val;

    /* reset the timer at first */
    gx6605s_write(gdev, GX6605S_TIM_CONTRL, GX6605S_TIM_CONTRL_RST);
    gx6605s_write(gdev, GX6605S_TIM_CONTRL, 0);

    gx6605s_write(gdev, GX6605S_TIM_CLKDIV, 53);

    val = UINT32_MAX - (1000000 / CONFIG_SYSTICK_FREQ) + 1;
    gx6605s_write(gdev, GX6605S_TIM_RELOAD, val);

    /* start the timer */
    gx6605s_write(gdev, GX6605S_TIM_CONFIG, GX6605S_TIM_CONFIG_IRQ_EN | GX6605S_TIM_CONFIG_EN);
    gx6605s_write(gdev, GX6605S_TIM_CONTRL, GX6605S_TIM_CONTRL_START);
}

static state gx6605s_timer_probe(struct platform_device *pdev, const void *pdata)
{
    struct gx6605s_device *gdev;
    resource_size_t start, size;
    state ret;

    if (platform_resource_type(pdev, 0) != RESOURCE_MMIO)
        return -EINVAL;

    gdev = dev_kzalloc(&pdev->dev, sizeof(*gdev), GFP_KERNEL);
    if (!gdev)
        return -ENOMEM;
    platform_set_devdata(pdev, gdev);

    start = platform_resource_start(pdev, 0);
    size = platform_resource_size(pdev, 0);
    gdev->mmio = dev_ioremap(&pdev->dev, start, size);
    if (!gdev->mmio)
        return -ENOMEM;

    gdev->irqchip = dt_irqchip_channel(pdev->dt_node, 0);
    if (!gdev->irqchip) {
        dev_err(&pdev->dev, "unable to request irq\n");
        return -ENODEV;
    }

    irq_request(10, 0, gx6605s_clkevt_handle, gdev, DRIVER_NAME);
    irqchip_pass(gdev->irqchip);

    gdev->clkevt.device = &pdev->dev;
    gdev->clkevt.ops = &gx6605s_clkevt_ops;
    gdev->clkevt.rating = CLOCK_RATING_GOOD;
    gdev->clkevt.capability = CLKEVT_CAP_PERIODIC | CLKEVT_CAP_ONESHOT;
    ret = clockevent_config_register(&gdev->clkevt, 0, 0x0f, 0x7fff);
    BUG_ON(ret);

    gx6605s_clkevt_hwinit(gdev);

#ifdef CONFIG_CLKSRC_GX6605S
    gdev->clksrc.device = &pdev->dev;
    gdev->clksrc.rating = CLOCK_RATING_GOOD;
    gdev->clksrc.ops = &gx6605s_clksrc_ops;
    ret = clocksource_config_register(&gdev->clksrc, 0, BIT_RANGE(31, 0));
    BUG_ON(ret);

    gx6605s_clksrc_hw_init(gdev);
#endif

    return -ENOERR;
}

static const struct dt_device_id gx6605s_timer_ids[] = {
    { .compatible = "nationalchip,gx6605s-timer" },
    { }, /* NULL */
};

static struct platform_driver gx6605s_timer_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .dt_table = gx6605s_timer_ids,
    .probe = gx6605s_timer_probe,
};

static state gx6605s_timer_init(void)
{
    return platform_driver_register(&gx6605s_timer_driver);
}
clocksource_initcall(gx6605s_timer_init);
