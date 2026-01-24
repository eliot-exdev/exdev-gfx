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

enum font_type {
    FONT_TYPE_MIA_1,
    FONT_TYPE_TOPAZ_8
};

struct Font {
    enum font_type type;
    int width;
    int height;
    int spacing;
    int num_characters;
    const struct TranslationTable *table;
    const char *characters;
};

typedef struct Font Font_t;

void font_init(Font_t *f, enum font_type t);

void font_deinit(Font_t *f);

const char *font_get_character(const Font_t *f, char c);

#ifdef __cplusplus
}
#endif

#endif//EXDEVGFX2_FONT_H
