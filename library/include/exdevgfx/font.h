/**
 * Copyright 2022 Andre Geisler (andre@exdev.de)
 */

#ifndef EXDEVGFX2_FONT_H
#define EXDEVGFX2_FONT_H

#ifdef __cplusplus
extern "C" {
#endif

struct TranslationTable {
    char c;
    unsigned char pos;
};

struct Font {
    int width;
    int height;
    int spacing;
    int num_characters;
    const struct TranslationTable *table;
    const char *characters;
};

typedef struct Font Font_t;

void font_init_mia_1(Font_t *f);

void font_deinit(Font_t *f);

const char *font_get_character(const Font_t *f, char c);

#ifdef __cplusplus
}
#endif

#endif //EXDEVGFX2_FONT_H
