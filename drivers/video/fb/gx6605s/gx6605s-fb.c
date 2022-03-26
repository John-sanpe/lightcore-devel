/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <string.h>
#include <initcall.h>
#include <kmalloc.h>
#include <ioremap.h>
#include <driver/platform.h>
#include <driver/video.h>
#include "gx6605s-video.h"

struct gx6605s_video_mode gx6605s_video_modes[GX6605S_HDMI_NR_MAX] = {
    [GX6605S_HDMI_480I] = {
        .name = "480I", .refresh = 60,
        .xres = 720, .yres = 480,
        .interlaced = true,
    },
    [GX6605S_HDMI_480P] = {
        .name = "480P", .refresh = 60,
        .xres = 720, .yres = 480,
        .interlaced = false,
    },
    [GX6605S_HDMI_576I] = {
        .name = "576I", .refresh = 60,
        .xres = 720, .yres = 576,
        .interlaced = true,
    },
    [GX6605S_HDMI_576P] = {
        .name = "576P", .refresh = 60,
        .xres = 720, .yres = 576,
        .interlaced = false,
    },
    [GX6605S_HDMI_720P_50HZ] = {
        .name = "720P_50HZ", .refresh = 50,
        .xres = 1280, .yres = 720,
        .interlaced = false,
    },
    [GX6605S_HDMI_720P_60HZ] = {
        .name = "720P_60HZ", .refresh = 60,
        .xres = 1280, .yres = 720,
        .interlaced = false,
    },
    [GX6605S_HDMI_1080I_50HZ] = {
        .name = "1080I_50HZ", .refresh = 50,
        .xres = 1920, .yres = 1080,
        .interlaced = true,
    },
    [GX6605S_HDMI_1080I_60HZ] = {
        .name = "1080I_60HZ", .refresh = 60,
        .xres = 1920, .yres = 1080,
        .interlaced = true,
    },
    [GX6605S_HDMI_1080P_50HZ] = {
        .name = "1080P_50HZ", .refresh = 50,
        .xres = 1920, .yres = 1080,
        .interlaced = false,
    },
    [GX6605S_HDMI_1080P_60HZ] = {
        .name = "1080P_60HZ", .refresh = 60,
        .xres = 1920, .yres = 1080,
        .interlaced = false,
    },
};

static state gx6605s_probe(struct platform_device *pdev, const void *pdata)
{
    struct gx6605s_device *gdev;
    state ret;

    gdev = dev_kzalloc(&pdev->dev, sizeof(*gdev), GFP_KERNEL);
    if (!gdev)
        return -ENOMEM;
    platform_set_devdata(pdev, gdev);

    if ((ret = gx6605s_hwinit(pdev)))
        return ret;

    gdev->video.mode_table = gx6605s_video_mode;
    gdev->video.cur_mode = &gx6605s_video_mode[0];
    gdev->video.device = &pdev->dev;
    gdev->video.ops = &gx6605s_ops;

    gx6605s_vpu_hwinit(gdev);
    gx6605s_osd_hwinit(gdev);
    gx6605s_vpu_zoom(gdev, 1, width, height, width, height, 0, 0);
    gx6605s_osd_enable(gdev, TRUE);

    return video_register(&gdev->video);
}

static void gx6605s_remove(struct platform_device *pdev)
{
    struct gx6605s_device *gdev = platform_get_devdata(pdev);

    gx6605s_osd_enable(gdev, false);

    iounmap(gdev->mmio);
    kfree(gdev->mmio);
    kfree(gdev->video.framebuffer);
    kfree(gdev);
}

static struct dt_device_id gx6605s_ids[] = {
    { .compatible = "nationalchip,gx6605s-vpu" },
    { }, /* NULL */
};

static struct platform_driver gx6605s_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .dt_table = gx6605s_ids,
    .probe = gx6605s_probe,
    .remove = gx6605s_remove,
};

static state gx6605s_fb_init(void)
{
    return platform_driver_register(&gx6605s_driver);
}
driver_initcall(gx6605s_fb_init);
