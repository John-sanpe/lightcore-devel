/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _LIBGRAPH_H_
#define _LIBGRAPH_H_

#include <types.h>
#include <stddef.h>

typedef uint16_t libgph_coord_t;
typedef uint32_t libgph_color_t;

struct libgph_point {
    libgph_coord_t xpos;
    libgph_coord_t ypos;
};

struct libgph_area {
    libgph_coord_t xpos1;
    libgph_coord_t xpos2;
    libgph_coord_t ypos1;
    libgph_coord_t ypos2;
};

struct libgph_rect {


    /* border */
    libgph_coord_t width;
    libgph_color_t color;

    /* outline */


    /* shadow */
};

#endif  /* _LIBGRAPH_H_ */
