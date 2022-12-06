/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _VTMACH_H_
#define _VTMACH_H_

#include <types.h>
#include <state.h>

enum vtmach_state {
    VTM_STATE_NORMAL        = 0,
    VTM_STATE_ESC           = 1,
    VTM_STATE_SQUARE        = 2,
    VTM_STATE_GETPARS       = 3,
    VTM_STATE_FUNCKEY       = 4,
	VTM_STATE_HASH          = 5,
    VTM_STATE_SETG0         = 6,
    VTM_STATE_SETG1         = 7,
    VTM_STATE_PERCENT       = 8,
    VTM_STATE_CSIIGNORE     = 9,
    VTM_STATE_NONSTD        = 10,
	VTM_STATE_PALETTE       = 11,
    VTM_STATE_OSC           = 12,
    VTM_STATE_APC           = 13,
    VTM_STATE_PM            = 14,
    VTM_STATE_DCS           = 15,
};

enum vtmach_vt102 {
    VTM_VT102_EPECMA        = 0,
    VTM_VT102_EPDEC         = 1,
    VTM_VT102_EPEQ          = 2,
    VTM_VT102_EPGT          = 3,
    VTM_VT102_EPLT          = 4,
};

enum vtmach_color {
    VTM_COLOR_DEFAULT       = 0,
    VTM_COLOR_BLACK         = 1,
    VTM_COLOR_DARK_RED      = 2,
    VTM_COLOR_DARK_GREEN    = 3,
    VTM_COLOR_DARK_YELLOW   = 4,
    VTM_COLOR_DARK_BLUE     = 5,
    VTM_COLOR_DARK_MAGENTA  = 6,
    VTM_COLOR_DARK_CYAN     = 7,
    VTM_COLOR_LIGHT_GRAY    = 8,
    VTM_COLOR_DARK_GRAY     = 9,
    VTM_COLOR_RED           = 10,
    VTM_COLOR_GREEN         = 11,
    VTM_COLOR_YELLOW        = 12,
    VTM_COLOR_BLUE          = 13,
    VTM_COLOR_MAGENTA       = 14,
    VTM_COLOR_CYAN          = 15,
    VTM_COLOR_WHITE         = 16,
};

enum vtmach_scroll {
	VTM_SCROLL_UP           = 0,
	VTM_SCROLL_DOWN         = 1,
};

struct vtmach_cursor {
    unsigned int xpos;
    unsigned int ypos;
    enum vtmach_color color;
};

struct vtmach_context {
    struct vtmach_ops *ops;
    enum vtmach_state vstate;
    enum vtmach_vt102 vt102;

    struct vtmach_cursor cur_main;
    struct vtmach_cursor cur_back;

    unsigned int con_top;
    unsigned int con_bottom;
    unsigned int con_size;
    unsigned int con_rows;
    unsigned int con_position;

    uint8_t *frame_buff;
    size_t frame_size;

    uint32_t origin_mode:1;

    unsigned int bell_pitch;
    unsigned int bell_duration;
};

struct vtmach_ops {
    state (*putc)(struct vtmach *vtm, unsigned int xpos, unsigned int ypos, char ch);
    state (*puts)(struct vtmach *vtm, unsigned int xpos, unsigned int ypos, int *str, unsigned int count);
    state (*clear)(struct vtmach *vtm, unsigned int xpos, unsigned int ypos, unsigned int width, unsigned int height);
	state (*scroll)(struct vtmach *vtm, enum vtmach_scroll dir, unsigned int top, unsigned int bottom, unsigned int lines);
	state (*blank)(struct vtmach *vtm, bool blank);
    state (*bell)(struct vtmach *vtm, unsigned int duration, unsigned int pitch);
};

#endif  /* _VTMACH_H_ */
