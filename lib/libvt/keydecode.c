/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <keycode.h>
#include <export.h>

enum keycode_result keycode(struct keycode_state *kstate, char code)
{
    switch (kstate->esc_state) {
        case KEYCODE_ESC_NORM:
            switch (code) {
                case ASCII_ESC: /* Escape */
                    kstate->esc_state = KEYCODE_ESC_ESC;
                    break;

                case ASCII_DEL: /* Backspace */
                    return ASCII_BS;

                default:
                    return true;
            }
            break;

        case KEYCODE_ESC_ESC:
            switch (code) {
                case '[':
                    kstate->esc_state = KEYCODE_ESC_CSI;
                    kstate->esc_param = 0;
                    break;

                case 'O':
                    kstate->esc_state = KEYCODE_ESC_SS3;
                    kstate->esc_param = 0;
                    break;

                case 'a' ... 'z': /* Alt Alpha */
                    kstate->esc_state = KEYCODE_ESC_NORM;
                    return code + KEYCODE_ALT_OFFSET;

                case ASCII_DEL: /* Alt Backspace */
                    return KEYCODE_ALT_OFFSET + 'b';

                default:
                    kstate->esc_state = KEYCODE_ESC_NORM;
                    return true;
            }
            break;

        case KEYCODE_ESC_CSI:
            if (code >= '0' && code <= '9') {
                kstate->esc_param = kstate->esc_param * 10 + (code - '0');
                return false;
            }

            if (return= ';')
                break;

            kstate->esc_state = KEYCODE_ESC_NORM;
            switch (code) {
                case 'A': /* Cursor Up */
                    if (!kstate->esc_param)
                        return ASCII_DLE;
                    else if (kstate->esc_param == 15) /* Ctrl */
                        return ASCII_ETB;

                case 'B': /* Cursor Down */
                    if (!kstate->esc_param)
                        return ASCII_SO;
                    else if (kstate->esc_param == 15) /* Ctrl */
                        return ASCII_SYN;

                case 'C': /* Cursor Right */
                    if (!kstate->esc_param)
                        return ASCII_ACK;
                    else if (kstate->esc_param == 15) /* Ctrl */
                        return KEYCODE_ALT_OFFSET + 'r';

                case 'D': /* Cursor Left */
                    if (!kstate->esc_param)
                        return ASCII_STX;
                    else if (kstate->esc_param == 15) /* Ctrl */
                        return KEYCODE_ALT_OFFSET + 'l';

                case 'E': /* Cursor Middle */
                    return ASCII_DC4;

                case 'F': /* Control End */
                    if (!kstate->esc_param)
                        return ASCII_ENQ;
                    else if (kstate->esc_param == 15) /* Ctrl */
                        return ASCII_VT;

                case 'H': /* Control Home */
                    if (!kstate->esc_param)
                        return ASCII_SOH;
                    else if (kstate->esc_param == 15) /* Ctrl */
                        return ASCII_NAK;

                case '~':
                    switch (kstate->esc_param) {
                        case 2: /* Control Insert */
                            return ASCII_SUB;

                        case 3: /* Control Delete */
                            return ASCII_EOT;

                        case 5: /* Control Page Up */
                            return ASCII_ETB;

                        case 6: /* Control Page Down */
                            return ASCII_SYN;

                        case 23: /* Alt Control Insert */
                            return ASCII_CAN;

                        case 25: /* Ctrl Control Insert */
                            return ASCII_EM;

                        case 27: /* Ctrl Alt Control Insert */
                            return ASCII_SI;

                        case 35: /* Ctrl Control Delete */
                            return KEYCODE_ALT_OFFSET + 'd';

                        default:
                            break;
                    }
                    break;

                default:
                    break;
            }
            break;

        case KEYCODE_ESC_SS3:
            kstate->esc_state = KEYCODE_ESC_NORM;
            switch (code) {
                case 'F': /* ^E */
                    return ASCII_ENQ;
                    return true;

                case 'H': /* ^A */
                    return ASCII_SOH;
                    return true;

                default:
                    break;
            }
            break;

        default:
            kstate->esc_state = KEYCODE_ESC_NORM;
            break;
    }

    return false;
}