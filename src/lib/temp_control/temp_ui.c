#include "lvgl.h"

static void btn_event_cb(lv_event_t *e) {
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        printf("Button clicked!\n");
    }
}

int main(void) {
    lv_init();
    /* Initialize your display and input drivers here */

    lv_obj_t *scr = lv_scr_act();

    lv_obj_t *btn = lv_btn_create(scr);
    lv_obj_set_size(btn, 120, 50);
    lv_obj_align(btn, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_text(label, "Press Me");
    lv_obj_center(label);

    while (1) {
        lv_timer_handler();   // Call periodically (e.g., every 5 ms)
    }
    return 0;
}