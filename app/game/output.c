#include "output.h"

#include <stdio.h>
#include <string.h>

#include "input.h"
#include "macros.h"
#include "util/minicurses.h"

#define SCREEN_SIZE_X (MAP_SIZE_X * CHARS_PER_BLOCK) + 2
#define SCREEN_SIZE_Y MAP_SIZE_Y + 2

static struct {
    uint8_t character[SCREEN_SIZE_Y][SCREEN_SIZE_X];
} screen_map;

static void fill_str(char* buf,
                     size_t size,
                     char header,
                     char fill_char,
                     size_t fill_n,
                     char trailer)
{
    if (!size--) {  // account for trailing zero
        return;
    }
    if (!size--) {
        goto finish;
    }
    *buf++ = header;
    while (fill_n--) {
        if (!size--) {
            goto finish;
        }
        *buf++ = fill_char;
    }
    if (!size--) {
        goto finish;
    }
    *buf++ = trailer;

finish:
    *buf++ = '\0';
}

void output_init(void)
{
    char tmp[SCREEN_SIZE_X + 1];

    mc_initscr();
    mc_setcursor(false);
    mc_set_bg(BACKGROUND_COLOR);
    mc_set_fg(mc_color_white);
    mc_setattr(mc_attr_normal);

    // Reset screen
    memset(&screen_map, 0, sizeof(screen_map));

    // Top line
    coord_t y = 0;
    mc_move(0, y++);
    fill_str(tmp,
             sizeof(tmp),
             mc_sym_ulcorner,
             mc_sym_hline,
             MAP_SIZE_X * CHARS_PER_BLOCK,
             mc_sym_urcorner);
    mc_putstr(tmp);

    // Map rows
    fill_str(tmp, sizeof(tmp), mc_sym_vline, ' ', MAP_SIZE_X * CHARS_PER_BLOCK, mc_sym_vline);
    size_t n = MAP_SIZE_Y;
    while (n--) {
        mc_move(0, y++);
        mc_putstr(tmp);
    }

    // Bottom line
    mc_move(0, y++);
    fill_str(tmp,
             sizeof(tmp),
             mc_sym_llcorner,
             mc_sym_hline,
             MAP_SIZE_X * CHARS_PER_BLOCK,
             mc_sym_lrcorner);
    mc_putstr(tmp);
}

void output_render(const game_map_t* map)
{
    for (coord_t y = 0; y < MAP_SIZE_Y; y++) {
        for (coord_t x = 0; x < MAP_SIZE_X; x++) {
            for (int i = 0; i < CHARS_PER_BLOCK; i++) {
                if (map->block[y][x] != screen_map.character[y][x * CHARS_PER_BLOCK + i]) {
                    mc_move(x * CHARS_PER_BLOCK + 1 + i, y + 1);
                    mc_color_t c = map->block[y][x];
                    mc_set_bg(c ? c : BACKGROUND_COLOR);
                    mc_putch(' ');
                    screen_map.character[y][x * CHARS_PER_BLOCK + i] = map->block[y][x];
                }
            }
        }
    }
}

static void output_text_msg(coord_t y, const char* str)
{
    coord_t x = MAX(((int)SCREEN_SIZE_X - (int)strlen(str)) / 2 - 1, 0);
    mc_move(x + 1, y + 1);
    mc_putstr(str);
    // Invalidate area, so it gets refreshed in output_render()
    for (coord_t xc = x; xc <= x + strlen(str) && xc < SCREEN_SIZE_X; xc++) {
        // Magic value never used in the actual map
        screen_map.character[y][xc] = 0xff;
    }
}

void output_text_box(const char** msgs)
{
    mc_setattr(mc_attr_bold);
    mc_set_bg(mc_color_black);
    mc_set_fg(mc_color_white);

    // Determine box dimensions
    size_t max_str_len = 0;
    coord_t y = 0;
    for (const char** msg = msgs; *msg; msg++) {
        max_str_len = MAX(max_str_len, strlen(*msg));
        y++;
    }
    y = MAX((MAP_SIZE_Y - y) / 2 - 1, 0);
    max_str_len = MIN(max_str_len, SCREEN_SIZE_X - 4);

    // Draw upper line
    char tmp[SCREEN_SIZE_X + 1];
    fill_str(tmp, sizeof(tmp), mc_sym_ulcorner, mc_sym_hline, max_str_len, mc_sym_urcorner);
    output_text_msg(y++, tmp);

    // Print content
    for (const char** msg = msgs; *msg; msg++) {
        const size_t l = strlen(*msg);
        int pad_left = (max_str_len - l) / 2;
        int pad_right = max_str_len - l - pad_left;
        snprintf(tmp,
                 sizeof(tmp),
                 "%c%*s%.*s%*s%c",
                 mc_sym_vline,
                 pad_left,
                 "",
                 max_str_len,
                 *msg,
                 pad_right,
                 "",
                 mc_sym_vline);
        output_text_msg(y++, tmp);
    }

    // Draw bottom line
    fill_str(tmp, sizeof(tmp), mc_sym_llcorner, mc_sym_hline, max_str_len, mc_sym_lrcorner);
    output_text_msg(y++, tmp);
}

static void show_key_bindings(void)
{
    mc_setattr(mc_attr_normal);
    mc_set_bg(mc_color_default);
    mc_set_fg(mc_color_default);
    const coord_t x = SCREEN_SIZE_X + 2;
    coord_t y = SCREEN_SIZE_Y - 1 - num_key_bindings;
    for (size_t i = 0; i < num_key_bindings; i++) {
        mc_move(x, y);
        mc_putstr(key_bindings[i].key_name);
        mc_move(x + 6, y);
        mc_putstr(key_bindings[i].cmd_name);
        y++;
    }
}

static void hide_key_bindings(void)
{
    mc_set_bg(mc_color_default);
    const coord_t x = SCREEN_SIZE_X + 2;
    coord_t y = SCREEN_SIZE_Y - 1 - num_key_bindings;
    for (size_t i = 0; i < num_key_bindings; i++) {
        mc_move(x, y++);
        mc_cleartoeol();
    }
}

void output_key_bindings(bool show)
{
    static bool visible = false;
    if (show != visible) {
        (show ? show_key_bindings : hide_key_bindings)();
        visible = show;
    }
}

void update_score_and_level(unsigned int score, unsigned int level)
{
    static unsigned int highscore = 0;
    highscore = MAX(score, highscore);

    mc_setattr(mc_attr_bold);
    mc_set_bg(mc_color_default);
    mc_set_fg(mc_color_default);
    const coord_t x = SCREEN_SIZE_X + 1;
    coord_t y = 5;
    char tmp[10];

    mc_move(x, y++);
    mc_putstr("   Level");
    snprintf(tmp, sizeof(tmp), "    %02u", level);
    mc_move(x, y++);
    mc_putstr(tmp);

    y++;
    mc_move(x, y++);
    mc_putstr("   Score");
    snprintf(tmp, sizeof(tmp), "  %07u", score);
    mc_move(x, y++);
    mc_putstr(tmp);

    y++;
    mc_move(x, y++);
    mc_putstr("High Score");
    snprintf(tmp, sizeof(tmp), "  %07u", highscore);
    mc_move(x, y++);
    mc_putstr(tmp);
}