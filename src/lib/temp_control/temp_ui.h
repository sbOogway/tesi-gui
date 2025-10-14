#ifndef BUTTON_DEMO_H
#define BUTTON_DEMO_H

#include "lvgl.h"

/* Initializes LVGL, display, and input drivers.
 * Call this once before creating any LVGL objects. */
void lvgl_init(void);

/* Creates a button with a label on the given parent object.
 * Returns a pointer to the created button. */
lv_obj_t *create_demo_button(lv_obj_t *parent, const char *label_text);

/* Event callback for the demo button.
 * You can replace the body with your own logic. */
void demo_button_event_cb(lv_event_t *e);

#endif /* BUTTON_DEMO_H */