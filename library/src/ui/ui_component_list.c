/**
* Copyright 2025 Andre Geisler (andre@exdev.de)
 */

#include "exdevgfx/ui/ui.h"
#include "exdevgfx/helper.h"
#include "exdevgfx/logger.h"

#include <assert.h>
#include <stdlib.h>

void ui_component_list_init(UIComponentList_t *self) {
    assert(self);

    self->components = NULL;
    self->size = 0;
}

void ui_component_list_destroy(UIComponentList_t *self) {
    assert(self);

    for (int i = 0; i < self->size; ++i) {
        self->components[i]->functions.destroy_func(self->components[i]);
        free(self->components[i]);
        self->components[i] = NULL;
    }
    free(self->components);
}

void ui_component_list_add(UIComponentList_t *self, UIComponent_t *component) {
    assert(self);
    assert(component);

    if (self->size > 0) {
        self->components = realloc(self->components, sizeof(UIComponent_t *) * (self->size + 1));
        if (!self->components) {
            log_warning("could allocate memory for component list");
            return;
        }
        self->size += 1;
    } else {
        self->components = malloc(sizeof(UIComponent_t *));
        self->size = 1;
    }
    self->components[self->size - 1] = component;
}
