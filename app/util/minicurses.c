#include "minicurses.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797
// https://man7.org/linux/man-pages/man4/console_codes.4.html

// clang-format off
static const char
  *seq_clear = "\033[2J",         // clear screen
  *seq_cleartoeol    = "\033[K",  // clear to end of line
  *seq_deleteline    = "\033[M",  // delete line
  *seq_load_g1       = "\033)0";  // load G1 character set
// clang-format on

static const char* key_seq[] = {
    "B",  "A",  "D",  "C",  "1~",  "3~",  "2~",  "6~",  "5~",  "4~",  "Z",   "[A",
    "[B", "[C", "[D", "[E", "17~", "18~", "19~", "20~", "21~", "23~", "24~",
};

static struct mc_ctx {
    uint8_t cursor_x;
    uint8_t cursor_y;
    struct display_params {
        mc_attr_t attr;
        mc_color_t fg;
        mc_color_t bg;
    } display_current, display_requested;
    bool g1_charset_active;
} ctx;

static void _putch(char c)
{
    write(STDOUT_FILENO, &c, sizeof(c));
}

static char _getch(void)
{
    char c;
    read(STDIN_FILENO, &c, sizeof(c));
    return c;
}

static void _putstr(const char* str)
{
    while (*str) {
        _putch(*str++);
    }
}

static void _updateattr(void)
{
    struct display_params *curr = &ctx.display_current, *req = &ctx.display_requested;
    if (!memcmp(curr, req, sizeof(*req))) {
        // Nothing to do
        return;
    }

    char tmp[80] = "\033[";
    size_t len = strlen(tmp);
    bool force_colors = false;

    if (curr->attr != req->attr) {
        bool attr_set = false;
        // Set or reset attributes to match the requested ones
        for (int i = 1; i <= 7; i++) {
            if (req->attr & (1 << i)) {
                // set the attribute
                len += snprintf(tmp + len,
                                sizeof(tmp) - len,
                                "%s%d",
                                (strlen(tmp) != 2) ? ";" : "",
                                i);
                attr_set = true;
            }
        }
        curr->attr = req->attr;
        if (!attr_set) {
            // reset all modes; must set colors again
            len += snprintf(tmp + len, sizeof(tmp) - len, "0");
            force_colors = true;
        }
    }

    if (curr->fg != req->fg || force_colors) {
        len +=
            snprintf(tmp + len, sizeof(tmp) - len, "%s3%d", (strlen(tmp) != 2) ? ";" : "", req->fg);
        curr->fg = req->fg;
    }
    if (curr->bg != req->bg || force_colors) {
        len +=
            snprintf(tmp + len, sizeof(tmp) - len, "%s4%d", (strlen(tmp) != 2) ? ";" : "", req->bg);
        curr->bg = req->bg;
    }

    len += snprintf(tmp + len, sizeof(tmp) - len, "m");
    _putstr(tmp);
}

static void _move(uint8_t x, uint8_t y)
{
    printf("\033[%d;%dH", y + 1, x + 1);
}

static int utf8_ext(uint8_t val)
{
    if (val >= 0xc2 && val <= 0xdf) {
        return 1;
    }
    if (val >= 0xe0 && val <= 0xef) {
        return 2;
    }
    if (val >= 0xf0 && val <= 0xf4) {
        return 3;
    }
    return 0;
}

static void update_g1(bool active)
{
    if (ctx.g1_charset_active != active) {
        _putch(active ? '\016' : '\017');
        ctx.g1_charset_active = active;
    }
}

void mc_putch(unsigned char c)
{
    static int unicode_cnt = 0;
    if (!unicode_cnt) {
        int ext = utf8_ext(c);
        if (ext) {
            // Print N characters raw & increment X only once
            unicode_cnt = ext;
            goto do_putch;
        }
    }
    if (unicode_cnt > 0) {
        _putch(c);
        unicode_cnt--;
        return;
    }

    bool use_g1 = (c >= 0x80 && c <= 0x9F);
    update_g1(use_g1);
    if (use_g1) {
        c -= 0x20;
    }

do_putch:
    _updateattr();
    _putch(c);
    ctx.cursor_x++;
}

void mc_putstr(const char* str)
{
    while (*str) {
        mc_putch(*str++);
    }
}

void mc_initscr(void)
{
    setvbuf(stdout, NULL, _IONBF, 0);  // turn off buffering on stdout
    _putstr(seq_load_g1);
    _putstr("\033[m");
    ctx.display_current = (struct display_params){
        .attr = mc_attr_normal,
        .fg = mc_color_default,
        .bg = mc_color_default,
    };
    ctx.display_requested = ctx.display_current;
    mc_clear();
    _move(0, 0);
}

void mc_setattr(mc_attr_t attr)
{
    ctx.display_requested.attr = attr;
}

void mc_set_fg(mc_color_t foreground)
{
    ctx.display_requested.fg = foreground;
}

void mc_set_bg(mc_color_t background)
{
    ctx.display_requested.bg = background;
}

void mc_move(uint8_t x, uint8_t y)
{
    if (ctx.cursor_x != x || ctx.cursor_y != y) {
        _move(x, y);
        ctx.cursor_x = x;
        ctx.cursor_y = y;
    }
}

void mc_deleterow(void)
{
    mc_move(0, ctx.cursor_y);
    _putstr(seq_deleteline);
}

void mc_clear(void)
{
    _updateattr();
    _putstr(seq_clear);
}

void mc_cleartoeol(void)
{
    _putstr(seq_cleartoeol);
}

void mc_setcursor(bool visible)
{
    printf("\033[?25%c", visible ? 'h' : 'l');
}

void mc_exitscr(void)
{
    _putch('\017');      // switch to G0 set
    mc_setcursor(true);  // show cursor
    mc_setattr(mc_attr_normal);
    mc_set_fg(mc_color_default);
    mc_set_bg(mc_color_default);
    _putch('\n');
}

int mc_getch(void)
{
    char tmp[4];

    int c = _getch();

    if (c == '\x1b') {
        c = _getch();

        if (c == '\x1b') {
            return key_escape;
        } else if (c == '[') {
            size_t i = 0;
            while (i < sizeof(tmp) - 1) {
                c = _getch();
                tmp[i++] = c;

                if ((c >= 'A' && c <= 'Z') || c == '~') {
                    break;
                }
            }
            tmp[i] = '\0';

            for (i = 0; i < sizeof(key_seq) / sizeof(key_seq[0]); i++) {
                if (!strcmp(tmp, key_seq[i])) {
                    return i + 0x80;
                }
            }

            return 0;
        } else {
            return 0;
        }
    }

    if (c == 0x7b) {
        return key_backspace;
    }
    return c;
}
