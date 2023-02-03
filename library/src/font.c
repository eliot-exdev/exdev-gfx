/**
 * Copyright 2022 Andre Geisler (andre@exdev.de)
 */

#include "exdevgfx/font.h"

#include <assert.h>

#define MIA_1_WIDTH 4
#define MIA_1_HEIGHT 7
#define MIA_1_SPACING 2
#define MIA_1_NUM_CHARS 11
static const char MIA_1_CHARACTERS[MIA_1_NUM_CHARS + 1][28] = {
        {1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1}, // joker
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // space
        {0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0}, // 0
        {0, 0, 1, 1, 0, 1, 0, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1}, // 1
        {0, 1, 1, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1}, // 2
        {0, 1, 1, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0}, // 3
        {1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0}, // 4
        {1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 0}, // 5
        {0, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0}, // 6
        {1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0}, // 7
        {0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0}, // 8
        {0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0} // 9
};

static const struct TranslationTable MIA_1_TRANSLATION_TABLE[MIA_1_NUM_CHARS] = {{' ', 1},
                                                                                 {'0', 2},
                                                                                 {'1', 3},
                                                                                 {'2', 4},
                                                                                 {'3', 5},
                                                                                 {'4', 6},
                                                                                 {'5', 7},
                                                                                 {'6', 8},
                                                                                 {'7', 9},
                                                                                 {'8', 10},
                                                                                 {'9', 11}};

void font_init_mia_1(Font_t *f) {
    assert(f);
    f->width = MIA_1_WIDTH;
    f->height = MIA_1_HEIGHT;
    f->spacing = MIA_1_SPACING;
    f->num_characters = MIA_1_NUM_CHARS;
    f->characters = MIA_1_CHARACTERS[0];
    f->table = MIA_1_TRANSLATION_TABLE;
}

void font_deinit(Font_t *) {}

const char *font_get_character(const Font_t *f, const char c) {
    assert(f);
    for (int i = 0; i < f->num_characters; ++i) {
        if (f->table[i].c == c) {
            return f->characters + f->table[i].pos * f->width * f->height;
        }
    }
    return f->characters; // return joker
}
