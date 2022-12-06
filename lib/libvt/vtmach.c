/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <vtmach.h>
#include <ascii.h>

static __always_inline void
vtm_bs(struct vtmach_context *vtmc)
{
    if (vtmc->cur.xpos) {
        vtmc->cur.xpos--;
    }
}

static __always_inline void
vtm_cr(struct vtmach_context *vtmc)
{

}

static __always_inline void
vtm_lf(struct vtmach_context *vtmc)
{

}

static __always_inline void
vtm_del(struct vtmach_context *vtmc)
{

}

static __always_inline void
vtm_gotoxy(struct vtmach_context *vtmc, unsigned int xpos, unsigned int ypos)
{
    unsigned int miypos, maypos;

    if (xpos >= vtmc->con_size)
        vtmc->cur_main.xpos = vtmc->con_size - 1;
    else
        vtmc->cur_main.xpos = xpos;

    if (vtmc->origin_mode) {
        miypos = vtmc->con_top;
        maypos = vtmc->con_bottom;
    } else {
        miypos = 0;
        maypos = vtmc->con_rows;
    }

    if (ypos < miypos)
        vtmc->cur_main.ypos = miypos;
    else if (ypos >= maypos)
        vtmc->cur_main.ypos = maypos;
    else
        vtmc->cur_main.ypos = ypos;
}

static __always_inline void
vtm_csi_j(struct vtmach_context *vtmc, unsigned var)
{

    switch (var) {
        case 0:
            break;

        case 1:
            break;

        case 3:
            fallthrough;

        case 2:
            break;

        default:
            return;
    }

}

static __always_inline void
vtm_csi_k(struct vtmach_context *vtmc, unsigned var)
{

}

static __always_inline void
vtm_csi_l(struct vtmach_context *vtmc, unsigned var)
{

}

static __always_inline void
vtm_csi_m(struct vtmach_context *vtmc, unsigned var)
{

}

static __always_inline void
vtm_csi_p(struct vtmach_context *vtmc, unsigned var)
{

}

static __always_inline void
vtm_csi_x(struct vtmach_context *vtmc, unsigned var)
{

}

static __always_inline void
vtm_csi_at(struct vtmach_context *vtmc, unsigned var)
{

}

static __always_inline void
vtm_csi_m(struct vtmach_context *vtmc, unsigned var)
{

}

static __always_inline void
corsor_save(struct vtmach_context *vtmc)
{
    memcpy(&vtmc->cur_back, &vtmc->cur_main, sizeof(struct vtmach_cursor));
}

static __always_inline void
corsor_restore(struct vtmach_context *vtmc)
{
    memcpy(&vtmc->cur_main, &vtmc->cur_back, sizeof(struct vtmach_cursor));
    vtm_gotoxy(vtmc, vtmc->cur_main.xpos, vtmc->cur_main.ypos);
}

static __always_inline bool
control_state(enum vtmach_state vtms)
{
    return vtms == VTM_STATE_OSC || vtms == VTM_STATE_APC ||
           vtms == VTM_STATE_PM  || vtms == VTM_STATE_DCS;
}

static void terminal_reset(struct vtmach_context *vtmc)
{
    vtmc->vstate = VTM_STATE_NORMAL;
    vtmc->vt102 = VTM_VT102_EPECMA;

    vtmc->con_top = 0;
    vtmc->con_bottom = vtmc->con_rows;


}

static void vtm_handle(struct vtmach_context *vtmc, char code)
{
    switch (code) {
        case ASCII_NUL: /* Null Char */
            return;

        case ASCII_BEL: /* ^G : Bell */
            if (control_state(vtmc->vstate))
                vtmc->vstate = VTM_STATE_NORMAL;
            else if (vtmc->bell_duration)
                vtmc->ops->bell(vtmc, vtmc->bell_duration, vtmc->bell_pitch);
            return;

        case ASCII_BS: /* ^H : Back Space */
            vtm_bs(vtmc);
            return;

        case ASCII_HT: /* ^I : Horizontal TAB */
            return;

        case ASCII_LF: /* ^J : Line Feed */
        case ASCII_VT: /* ^K : Vertical Tab */
        case ASCII_FF: /* ^L : Form Feed */
            vtm_lf(vtmc);


            fallthrough;

        case ASCII_CR: /* ^M : Carriage Return */
            vtm_cr(vtmc);
            return;

        case ASCII_SO: /* ^N : Shift Out / X-On */
            return;

        case ASCII_SI: /* ^O : Shift In / X-Off */
            return;

        case ASCII_DLE: /* ^P : Data Line Escape */
            break;

        case ASCII_DC1: /* ^Q : Device Control 1 */
            break;

        case ASCII_DC2: /* ^R : Device Control 2 */
            break;

        case ASCII_DC3: /* ^S : Device Control 3 */
            break;

        case ASCII_DC4: /* ^T : Device Control 4 */
            break;

        case ASCII_NAK: /* ^U : Negative Acknowledgement */
            break;

        case ASCII_SYN: /* ^V : Synchronous Idle */
            break;

        case ASCII_ETB: /* ^W : End of Transmit Block */
            break;

        case ASCII_EM: /* ^Y : End of Medium */
            return;

        case ASCII_CAN: /* ^X : Cancel */
        case ASCII_SUB: /* ^Z : Substitute */
            vtmc->vstate = VTM_STATE_SQUARE;
            return;

        case ASCII_ESC: /* Escape */
            vtmc->vstate = VTM_STATE_ESC;
            return;

        case ASCII_FS: /* File Separator */
            break;

        case ASCII_GS: /* Group Separator */
            break;

        case ASCII_RS: /* Record Separator */
            break;

        case ASCII_US: /* Unit Separator */
            break;

        case ASCII_DEL:
            vtm_del(vtmc);
            return;

        default:
            break;
    }

    switch (vtmc->vstate) {
        case VTM_STATE_ESC:
            switch (code) {
                case '[':
                    vtmc->vstate = VTM_STATE_SQUARE;
                    return;

                case ']':
                    vtmc->vstate = VTM_STATE_NONSTD;
                    return;

                case '_':
                    vtmc->vstate = VTM_STATE_APC;
                    return;

                case '^':
                    vtmc->vstate = VTM_STATE_PM;
                    return;

                case '%':
                    vtmc->vstate = VTM_STATE_PERCENT;
                    return;

                case 'E':
                    vtm_cr(vtmc);
                    vtm_lf(vtmc);
                    return;

                case 'M':
                    vtm_ri(vtmc);
                    return;

                case 'D':
                    vtm_lf(vtmc);
                    return;

                case 'H':
                    return;

                case 'P':
                    vtmc->vstate = VTM_STATE_DCS;
                    return;

                case 'Z':
                    return;

                case '7':
                    corsor_save(vtmc);
                    return;

                case '8':
                    corsor_restore(vtmc);
                    return;

                case '(':
                    vtmc->vstate = VTM_STATE_SETG0;
                    return;

                case ')':
                    vtmc->vstate = VTM_STATE_SETG1;
                    return;

                case '#':
                    vtmc->vstate = VTM_STATE_HASH;
                    return;

                case 'c':
                    return;

                case '>':
                    return;

                case '=':
                    return;

                default:
                    vtmc->vstate = VTM_STATE_NORMAL;
                    return;
            }

        case VTM_STATE_NONSTD:
            return;

        case VTM_STATE_PALETTE:
            return;

        case VTM_STATE_SQUARE:
            return;

        case VTM_STATE_GETPARS:
            if (code == ';' && vtmc->par_nr < VT_PAR_NR) {
                vtmc->par_nr++;
                return;
            } else if (isdigit(code)) {
                vtmc->par[vtmc->par_nr] *= 10;
                vtmc->par[vtmc->par_nr] += code - '0';
            }

            if (code >= 0x20 && code <= 0x3f) {
                vtmc->vstate = VTM_STATE_CSIIGNORE;
                return 0;
            }

            vtmc->vstate = VTM_STATE_NORMAL;
            switch (code) {
                case 'h':
                    return;

                case 'l':
                    return;

                case 'c':
                    return;

                case 'm':
                    return;

                case 'n':
                    return;

            }

            switch (code) {
                case 'G': case '`': /* Horizontal position */
                    if (vtmc->par[0])
                        vtmc->par[0]--;
                    vtm_goto(vtmc, vtmc->par[0], vtmc->cur.ypos);
                    return;

                case 'd': /* Vertical position */
                    if (vtmc->par[0])
                        vtmc->par[0]--;
                    vtm_goto(vtmc, vtmc->cur.xpos, vtmc->par[0]);
                    return;

                case 'H': case 'f':  /* Horizontal and Vertical position */
                    if (vtmc->par[0])
                        vtmc->par[0]--;
                    if (vtmc->par[1])
                        vtmc->par[1]--;
                    vtm_goto(vtmc, vtmc->par[1], vtmc->par[0]);
                    return;

                case 'A': /* Upper offset */
                    if (!vtmc->par[0])
                        vtmc->par[0]--;
                    vtm_goto(vtmc, vtmc->cur.xpos, vtmc->cur.ypos - vtmc->par[0]);
                    return;

                case 'B': case 'e': /* Lower offset */
                    if (!vtmc->par[0])
                        vtmc->par[0]--;
                    vtm_goto(vtmc, vtmc->cur.xpos, vtmc->cur.ypos + vtmc->par[0]);
                    return;

                case 'D': /* Left offset */
                    if (!vtmc->par[0])
                        vtmc->par[0]--;
                    vtm_goto(vtmc, vtmc->cur.xpos - vtmc->par[0], vtmc->cur.ypos);
                    return;

                case 'C': case 'a': /* Right offset */
                    if (!vtmc->par[0])
                        vtmc->par[0]--;
                    vtm_goto(vtmc, vtmc->cur.xpos + vtmc->par[0], vtmc->cur.ypos);
                    return;

                case 'E': /* Vertical position */
                    if (!vtmc->par[0])
                        vtmc->par[0]--;
                    vtm_goto(vtmc, 0, vtmc->cur.ypos + vtmc->par[0]);
                    return;

                case 'F': /* Vertical position */
                    if (!vtmc->par[0])
                        vtmc->par[0]--;
                    vtm_goto(vtmc, 0, vtmc->cur.ypos + vtmc->par[0]);
                    return;

                case 'r': /* Reset scrolling region */
                    if (!vtmc->par[0])
                        vtmc->par[0]++;
                    if (!vtmc->par[1])
                        vtmc->par[1] = vtmc->rows;
                    if (vtmc->par[0] < vtmc->par[1] &&
                        vtmc->par[1] <= vtmc->rows) {
                        vtmc->top = vtmc->par[0] - 1;
                        vtmc->bottom = vtmc->par[1];
                        vtm_goto(vtmc, 0, 0);
                    }
                    return;

                case 'J':
                    vtm_csi_J(vtmc, vtmc->par[0]);
                    return;

                case 'K':
                    vtm_csi_K(vtmc, vtmc->par[0]);
                    return;

                case 'L':
                    vtm_csi_L(vtmc, vtmc->par[0]);
                    return;

                case 'M':
                    vtm_csi_M(vtmc, vtmc->par[0]);
                    return;

                case 'P':
                    vtm_csi_P(vtmc, vtmc->par[0]);
                    return;

                case 'X':
                    vtm_csi_X(vtmc, vtmc->par[0]);
                    return;

                case '@':
                    vtm_csi_at(vtmc, vtmc->par[0]);
                    return;

                case 'm':
                    vtm_csi_m(vtmc, vtmc->par[0]);
                    return;

                case 'c':
                    return;

                case 'g':
                    return;

                case 'q':
                    return;

                case 's':
                    vtm_cur_save(vtmc);
                    return;

                case 'u':
                    vtm_cur_restore(vtmc);
                    return;

                case ']':
                    return;
            }
            return;

        case VTM_STATE_CSIIGNORE:
            if (code >= 20 && code <= 0x3f)
                return;
            vtmc->vstate = VTM_STATE_NORMAL;
            return;

        case VTM_STATE_PERCENT:
            vtmc->vstate = VTM_STATE_NORMAL;
            return;

        case VTM_STATE_FUNCKEY:
            vtmc->vstate = VTM_STATE_NORMAL;
            return;

        case VTM_STATE_HASH:
            vtmc->vstate = VTM_STATE_NORMAL;
            return;

        case VTM_STATE_SETG0:
            vtmc->vstate = VTM_STATE_NORMAL;
            return;

        case VTM_STATE_SETG1:
            vtmc->vstate = VTM_STATE_NORMAL;
            return;

        case VTM_STATE_APC:
            return;

        case VTM_STATE_OSC:
            return;

        case VTM_STATE_PM:
            return;

        case VTM_STATE_DCS:
            return;

        default:
            vtmc->vstate = VTM_STATE_NORMAL;
    }
}

static bool vtm_is_control(struct vtmach_context *vtmc, char code)
{
    if (vtmc->vstate != VTM_STATE_NORMAL)
        return true;

    if (code < 32)
        return true;

    if (code == 127)
        return true;

    if (code == 128 + 27)
        return true;

    return false;
}

void control(struct vtmach_context *vtmc, char code)
{
    if (vtm_is_control(vtmc, code)) {
        vtm_handle(vtmc, code);
    }
}


void vtm_handle(struct vtmach_context *vtmc, char code)
{


}
