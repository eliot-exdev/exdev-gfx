/**
 * Copyright 2025 Andre Geisler (andre@exdev.de)
 */

#define EXDEVGFX2_LOG_LEVEL 2
#include "exdevgfx/logger.h"
#include "exdevgfx/ui/ui.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

void ui_text_init(UIText_t *self, const int x, const int y, const int width, const int height, const char *text, const Font_t *font) {
    assert(self);
    assert(font);

    ui_component_init(&self->base, x, y, width, height);
    self->base.type = UI_COMPONENT_TEXT;
    self->base.functions.destroy_func = (void (*)(void *)) &ui_text_destroy;
    self->base.functions.paint_func = (int (*)(void *, Framebuffer8Bit_t *, int, int, int, int, void *)) &ui_text_paint;

    if (text) {
        self->properties.text = malloc(strlen(text) + 1);
        strcpy(self->properties.text, text);
    } else {
        self->properties.text = NULL;
    }
    self->properties.font = font;
    self->properties.text_color = PEN_INDEX_YELLOW;
}

UIText_t *ui_text_create(const int x, const int y, const int width, const int height, const char *text, const Font_t *font) {
    assert(font);

    UIText_t *self = malloc(sizeof(UIText_t));
    ui_text_init(self, x, y, width, height, text, font);

    return self;
}

void ui_text_destroy(UIText_t *self) {
    assert(self);

    ui_component_destroy(&self->base);

    if (self->properties.text) {
        free(self->properties.text);
        self->properties.text = NULL;
    }
    self->properties.font = NULL;
}

int ui_text_paint(UIText_t *self, Framebuffer8Bit_t *fb, const int x_offset, const int y_offset, const int width, const int height, void *usr_ptr) {
    assert(self);
    assert(fb);

    const int res = self->base.flags.dirty_flag;
    const int x = self->base.properties.x + x_offset;
    const int y = self->base.properties.y + y_offset;

    // draw base
    ui_component_paint(&self->base, fb, x_offset, y_offset, width, height, usr_ptr);
    if (res) {
        if (self->properties.text) {
            framebuffer_8bit_draw_text(fb,
                                       self->properties.font,
                                       self->properties.text,
                                       (int) strlen(self->properties.text),
                                       self->properties.text_color,
                                       x + 2,
                                       y + (self->base.properties.height - self->properties.font->height) / 2);
        }
    }
    return res;
}

void ui_text_update_text(UIText_t *self, const char *text) {
    assert(self);

    if (!text) {
        free(self->properties.text);
        self->properties.text = NULL;
        self->base.functions.dirty_function(self);
        return;
    }

    if (!self->properties.text) {
        self->properties.text = malloc(strlen(text) + 1);
    } else if (strlen(text) != strlen(self->properties.text)) {
        free(self->properties.text);
        self->properties.text = malloc(strlen(text) + 1);
    }

    strcpy(self->properties.text, text);
    self->base.functions.dirty_function(self);
}
