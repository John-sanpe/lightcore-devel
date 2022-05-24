/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include "kshell.h"
#include <string.h>
#include <sched.h>
#include <ascii.h>
#include <kmalloc.h>
#include <driver/buzzer.h>
#include <proc.h>

#define READLINE_BUFFER_DEF     64
#define READLINE_WORKSPACE_DEF  64

#define READLINE_BELL_PITCH     750
#define READLINE_BELL_DURATION	(CONFIG_SYSTICK_FREQ / 8)

unsigned int readline_read(struct readline_state *rstate, char *str, unsigned int len)
{
    return rstate->read(str, len, rstate->data);
}

void readline_write(struct readline_state *rstate, const char *str, unsigned int len)
{
    rstate->write(str, len, rstate->data);
}

static void readline_state_setup(struct readline_state *rstate)
{
    rstate->pos = 0;
    rstate->len = 0;
    rstate->curr = NULL;
    rstate->esc_state = READLINE_ESC_NORM;
}

static inline void readline_cursor_save(struct readline_state *rstate)
{
    readline_write(rstate, "\e[s", 3);
}

static inline void readline_cursor_restore(struct readline_state *rstate)
{
    readline_write(rstate, "\e[u", 3);
}

static void readline_cursor_left(struct readline_state *rstate)
{
    if (rstate->pos) {
        readline_write(rstate, "\e[D", 3);
        --rstate->pos;
    }
}

static void readline_cursor_right(struct readline_state *rstate)
{
    if (rstate->pos < rstate->len) {
        readline_write(rstate, "\e[C", 3);
        ++rstate->pos;
    }
}

static void readline_cursor_offset(struct readline_state *rstate, unsigned int offset)
{
    while (rstate->pos != offset) {
        if (rstate->pos > offset)
            readline_cursor_left(rstate);
        else if (rstate->pos < offset)
            readline_cursor_right(rstate);
    }
}

static void readline_cursor_home(struct readline_state *rstate)
{
    readline_cursor_offset(rstate, 0);
}

static void readline_cursor_end(struct readline_state *rstate)
{
    readline_cursor_offset(rstate, rstate->len);
}

static void readline_fill(struct readline_state *rstate, unsigned int len)
{
    while (len--)
        readline_write(rstate, " ", 1);
}

static state readline_insert(struct readline_state *rstate, const char *str, unsigned int len)
{
    if (rstate->len + len >= rstate->bsize) {
        unsigned int nbsize = rstate->bsize;
        void *nblk;

        while (rstate->len + len >= nbsize)
            nbsize *= 2;

        nblk = krealloc(rstate->buff, nbsize, GFP_KERNEL);
        if (!nblk)
            return -ENOMEM;

        rstate->buff = nblk;
        rstate->bsize = nbsize;
    }

    memmove(rstate->buff + rstate->pos + len, rstate->buff + rstate->pos, rstate->len - rstate->pos + 1);
    memmove(rstate->buff + rstate->pos, str, len);
    rstate->pos += len;
    rstate->len += len;

    readline_write(rstate, &rstate->buff[rstate->pos - len], len);
    readline_cursor_save(rstate);
    readline_write(rstate, &rstate->buff[rstate->pos], rstate->len - rstate->pos);
    readline_cursor_restore(rstate);
    return -ENOERR;
}

static void readline_delete(struct readline_state *rstate, unsigned int len)
{
    min_adj(len, rstate->len - rstate->pos);

    memmove(rstate->buff + rstate->pos, rstate->buff + rstate->pos + len, rstate->len - rstate->pos + 1);
    rstate->len -= len;

    readline_cursor_save(rstate);
    readline_write(rstate, &rstate->buff[rstate->pos], rstate->len - rstate->pos);
    readline_fill(rstate, len);
    readline_cursor_restore(rstate);
}

static void readline_backspace(struct readline_state *rstate, unsigned int len)
{
    unsigned int pos = rstate->pos;

    min_adj(len, rstate->pos);
    readline_cursor_offset(rstate, pos - len);
    readline_delete(rstate, len);
}

static void readline_clear(struct readline_state *rstate)
{
    readline_state_setup(rstate);
    readline_write(rstate, "\e[2J\e[1;1H", 10);
    readline_write(rstate, rstate->prompt, rstate->plen);
}

static state readline_save_workspace(struct readline_state *rstate)
{
    if (rstate->len > rstate->worksize) {
        unsigned int nbsize = rstate->worksize;
        void *nblk;

        while (rstate->len > nbsize)
            nbsize *= 2;

        nblk = kmalloc(nbsize, GFP_KERNEL);
        if (!nblk)
            return -ENOMEM;

        kfree(rstate->workspace);
        rstate->workspace = nblk;
        rstate->worksize = nbsize;
    }

    rstate->worklen = rstate->len;
    memcpy((char *)rstate->workspace, rstate->buff, rstate->len);

    return -ENOERR;
}

static void readline_restory_workspace(struct readline_state *rstate)
{
    if (rstate->worklen)
        readline_insert(rstate, rstate->workspace, rstate->worklen);
}

static state readline_history_add(struct readline_state *rstate, const char *cmd, unsigned int len)
{
    struct readline_history *history;

    if (rstate->worklen)
        rstate->worklen = 0;

    history = list_first_entry_or_null(&rstate->history, struct readline_history, list);
    if (history && history->len == len && !strncmp(history->cmd, cmd, len))
        return -ENOERR;

    history = kmalloc(sizeof(*history) + len, GFP_KERNEL);
    if (!history)
        return -ENOMEM;

    history->len = len;
    list_head_init(&history->list);

    memcpy(history->cmd, cmd, len);
    list_add(&rstate->history, &history->list);

    return -ENOERR;
}

static struct readline_history *readline_history_prev(struct readline_state *rstate, const char *cmd, unsigned int len)
{
    struct readline_history *prev;
    state retval;

    if (!rstate->curr) {
        retval = readline_save_workspace(rstate);
        if (retval)
            return ERR_PTR(retval);
    }

    if (rstate->worklen) {
        if (rstate->curr) for (prev = list_next_entry(rstate->curr, list);
                               !list_entry_check_head(prev, &rstate->history, list);
                               prev = list_next_entry(prev, list)) {
            if (!strncmp(rstate->workspace, prev->cmd, rstate->worklen))
                goto finish;

        } else for (prev = list_first_entry(&rstate->history, struct readline_history, list);
                    !list_entry_check_head(prev, &rstate->history, list);
                    prev = list_next_entry(prev, list)) {
            if (!strncmp(rstate->workspace, prev->cmd, rstate->worklen))
                goto finish;
        }
        return NULL;
    }

    else if (rstate->curr) {
        prev = list_next_entry(rstate->curr, list);
        if (list_entry_check_head(prev, &rstate->history, list))
            return NULL;
    } else {
        prev = list_first_entry_or_null(&rstate->history,
                    struct readline_history, list);
    }

finish:
    rstate->curr = prev;
    return prev;
}

static struct readline_history *readline_history_next(struct readline_state *rstate)
{
    struct readline_history *next;

    if (!rstate->curr)
        return NULL;

    if (rstate->worklen) {
        for (next = list_prev_entry(rstate->curr, list);
             !list_entry_check_head(next, &rstate->history, list);
             next = list_prev_entry(next, list)) {
            if (!strncmp(rstate->workspace, next->cmd, rstate->worklen))
                goto finish;
        }
        next = NULL;
        goto finish;
    }

    else {
        next = list_prev_entry(rstate->curr, list);
        if (list_entry_check_head(next, &rstate->history, list))
            next = NULL;
    }

finish:
    rstate->curr = next;
    return next;
}

static state readline_complete(struct readline_state *rstate)
{
    struct kshell_command *cmd, *tmp;
    unsigned int line, count = 0;
    const char *start;
    state retval;

    LIST_HEAD(complete);
    start = strrnchr(rstate->buff, rstate->pos - 1, ' ');
    line = start ? ++start - rstate->buff : 0;

    if (line >= rstate->pos)
        return -ENOERR;

    spin_lock(&kshell_lock);
    list_for_each_entry(cmd, &kshell_list, list) {
        if ((!start && strncmp(cmd->name, rstate->buff, rstate->pos)) ||
            (start && strncmp(cmd->name, start, rstate->pos - line)))
            continue;
        count++;
        list_add(&complete, &cmd->complete);
    }
    spin_unlock(&kshell_lock);

    if (count == 0)
        return -ENOERR;

    cmd = list_first_entry(&complete, struct kshell_command, complete);
    if (count == 1) {
        readline_backspace(rstate, rstate->pos - line);
        retval = readline_insert(rstate, cmd->name, strlen(cmd->name));
        readline_insert(rstate, " ", 1);
        goto finish;
    }

    readline_save_workspace(rstate);
    readline_write(rstate, "\n", 1);

    while (count) {
        for (line = 4; count && line; --count, --line) {
            readline_write(rstate, cmd->name, strlen(cmd->name));
            readline_write(rstate, "\t", 1);
            cmd = list_next_entry(cmd, complete);
        }
        readline_write(rstate, "\n", 1);
    }

    readline_state_setup(rstate);
    readline_write(rstate, rstate->prompt, rstate->plen);
    readline_restory_workspace(rstate);

finish:
    list_for_each_entry_safe(cmd, tmp, &complete, complete)
        list_del(&cmd->complete);

    return retval;
}

static bool readline_handle(struct readline_state *state, char code)
{
    struct readline_history *history;
    unsigned int tmp;

    switch (code) {
        case ASCII_NUL: /* Null Char */
            break;

        case ASCII_SOH: /* ^A : Start of Heading */
            readline_cursor_home(state);
            break;

        case ASCII_STX: /* ^B : Start of Text */
            readline_cursor_left(state);
            break;

        case ASCII_ETX: /* ^C : End of Text */
            state->len = state->pos = 0;
            return true;

        case ASCII_EOT: /* ^D : End of Transmission */
            if (state->pos < state->len)
                readline_delete(state, 1);
            readline_save_workspace(state);
            state->curr = NULL;
            break;

        case ASCII_ENQ: /* ^E : Enquiry */
            readline_cursor_end(state);
            break;

        case ASCII_ACK: /* ^F : Acknowledgment */
            readline_cursor_right(state);
            break;

        case ASCII_BEL: /* ^G : Bell */
            buzzer_beep(READ_ONCE(default_buzzer),
                READLINE_BELL_PITCH, READLINE_BELL_DURATION);
            break;

        case ASCII_BS: /* ^H : Back Space */
            if (state->pos)
                readline_backspace(state, 1);
            readline_save_workspace(state);
            state->curr = NULL;
            break;

        case ASCII_HT: /* ^I : Horizontal TAB */
            if (state->len)
                readline_complete(state);
            break;

        case ASCII_VT: /* ^K : Vertical Tab */
            break;

        case ASCII_FF: /* ^L : Form Feed */
            readline_clear(state);
            break;

        case ASCII_LF: /* ^J : Line Feed */
        case ASCII_CR: /* ^M : Carriage Return */
            if (state->len)
                readline_history_add(state, state->buff, state->len);
            return true;

        case ASCII_SO: /* ^N : Shift Out / X-On */
            history = readline_history_next(state);
            readline_backspace(state, state->len);
            if (history)
                readline_insert(state, history->cmd, history->len);
            else
                readline_restory_workspace(state);
            break;

        case ASCII_SI: /* ^O : Shift In / X-Off */
            break;

        case ASCII_DLE: /* ^P : Data Line Escape */
            history = readline_history_prev(state, state->buff, state->len);
            if (history) {
                readline_backspace(state, state->len);
                readline_insert(state, history->cmd, history->len);
            }
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
            if (state->pos) {
                tmp = state->pos;
                readline_cursor_home(state);
                readline_delete(state, tmp);
            }
            readline_save_workspace(state);
            state->curr = NULL;
            break;

        case ASCII_SYN: /* ^V : Synchronous Idle */
            break;

        case ASCII_ETB: /* ^W : End of Transmit Block */
            break;

        case ASCII_CAN: /* ^X : Cancel */
            break;

        case ASCII_EM: /* ^Y : End of Medium */
            break;

        case ASCII_SUB: /* ^Z : Substitute */
            break;

        default:
            readline_insert(state, &code, 1);
            readline_save_workspace(state);
            state->curr = NULL;
    }

    return false;
}

static bool readline_getcode(struct readline_state *state, char *code)
{

    if (!readline_read(state, code, 1))
        return false;

    switch (state->esc_state) {
        case READLINE_ESC_NORM:
            switch (*code) {
                case ASCII_ESC: /* Escape */
                    state->esc_state = READLINE_ESC_ESC;
                    break;

                case ASCII_DEL: /* Back Space */
                    *code = ASCII_BS;
                    return true;

                default:
                    return true;
            }
            break;

        case READLINE_ESC_ESC:
            switch (*code) {
                case '[':
                    state->esc_state = READLINE_ESC_CSI;
                    state->esc_param = 0;
                    break;

                case 'O':
                    state->esc_state = READLINE_ESC_SS3;
                    state->esc_param = 0;
                    break;

                default:
                    state->esc_state = READLINE_ESC_NORM;
                    return true;
            }
            break;

        case READLINE_ESC_CSI:
            if (*code >= '0' && *code <= '9') {
                state->esc_param = state->esc_param * 10 + (*code - '0');
                return false;
            } else {
                if (*code == ';')
                    break;

                state->esc_state = READLINE_ESC_NORM;
                switch (*code) {
                    case 'A': /* Cursor Up */
                        *code = ASCII_DLE;
                        return true;

                    case 'B': /* Cursor Down */
                        *code = ASCII_SO;
                        return true;

                    case 'C': /* Cursor Right */
                        *code = ASCII_ACK;
                        return true;

                    case 'D': /* Cursor Left */
                        *code = ASCII_STX;
                        return true;

                    case 'E': /* Cursor Middle */
                        *code = ASCII_CR;
                        return true;

                    case 'F': /* Control End */
                        *code = ASCII_ENQ;
                        return true;

                    case 'H': /* Control Home */
                        *code = ASCII_SOH;
                        return true;

                    case '~':
                        switch (state->esc_param) {
                            case 2: /* Control Insert */
                                *code = ASCII_SOH;
                                return true;

                            case 3: /* Control Delete */
                                *code = ASCII_EOT;
                                return true;

                            case 5: /* Control Page Up */
                                *code = ASCII_DLE;
                                return true;

                            case 6: /* Control Page Down */
                                *code = ASCII_SO;
                                return true;

                            default:
                                break;
                        }
                        break;

                    default:
                        break;
                }
            }
            break;

        case READLINE_ESC_SS3:
            state->esc_state = READLINE_ESC_NORM;
            switch (*code) {
                case 'F': /* ^E */
                    *code = ASCII_ENQ;
                    return true;

                case 'H': /* ^A */
                    *code = ASCII_SOH;
                    return true;

                default:
                    break;
            }
            break;

        default:
            state->esc_state = READLINE_ESC_NORM;
            break;
    }

    return false;
}

char *readline(struct readline_state *state, const char *prompt)
{
    char code;

    readline_state_setup(state);

    if (prompt) {
        state->prompt = prompt;
        state->plen = strlen(prompt);
        readline_write(state, state->prompt, state->plen);
    }

    for (;;) {
        if (!readline_getcode(state, &code)) {
            scheduler_yield();
        } else {
            if (readline_handle(state, code))
                break;
        }
    }

    readline_write(state, "\n", 1);
    state->buff[state->len] = '\0';
    return state->buff;
}

struct readline_state *readline_alloc(kshell_read_t read, kshell_write_t write, void *data)
{
    struct readline_state *state;

    state = kzalloc(sizeof(*state), GFP_KERNEL);
    if (!state)
        return NULL;

    state->read = read;
    state->write = write;
    state->data = data;

    state->bsize = READLINE_BUFFER_DEF;
    state->buff = kmalloc(state->bsize, GFP_KERNEL);
    if (!state->buff)
        return NULL;

    state->worksize = READLINE_WORKSPACE_DEF;
    state->workspace = kmalloc(state->worksize, GFP_KERNEL);
    if (!state->workspace)
        return NULL;

    list_head_init(&state->history);
    return state;
}

void readline_free(struct readline_state *state)
{
    struct readline_history *history, *next;

    list_for_each_entry_safe(history, next, &state->history, list) {
        list_del(&history->list);
        kfree(history);
    }

    kfree(state->buff);
    kfree(state->workspace);
    kfree(state);
}
