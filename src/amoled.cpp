#include <Arduino.h>
#include "lvgl.h"      /* https://github.com/lvgl/lvgl.git */
#include "rm67162.h"
#include "amoled.h"

static lv_disp_draw_buf_t draw_buf;
static lv_color_t *buf;

objects dobj; // Display objects

LV_IMG_DECLARE(driftfun_raceteam_2024);
LV_FONT_DECLARE(GIGA);

#define MAX_SCREENS 10
lv_obj_t* screens[MAX_SCREENS];
int current_screen_index = 0;

void my_disp_flush(lv_disp_drv_t *disp,
                   const lv_area_t *area,
                   lv_color_t *color_p)
{
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);
    lcd_PushColors(area->x1, area->y1, w, h, (uint16_t *)&color_p->full);
    lv_disp_flush_ready(disp);
}

void init_screens(void) {
    for (int i = 0; i < MAX_SCREENS; ++i) {
        screens[i] = lv_obj_create(NULL);
        // Customize each screen as needed
        if (i == 0) {
            lv_obj_t * icon = lv_img_create(screens[i]);
            lv_img_set_src(icon, &driftfun_raceteam_2024);
            lv_obj_align(icon, LV_ALIGN_CENTER, 0, 0);
        } else if (i == 1) {
            // show text
            lv_obj_t *label1 = lv_label_create(screens[i]);
            lv_label_set_long_mode(label1, LV_LABEL_LONG_WRAP);     /*Break the long lines*/
            lv_label_set_recolor(label1, true);                      /*Enable re-coloring by commands in the text*/
            lv_label_set_text(label1, "#0000ff Re-color# #ff00ff words# #ff0000 of a# label, align the lines to the center "
                            "and wrap long text automatically.");
            lv_obj_set_width(label1, 150);  /*Set smaller width to make the lines wrap*/
            lv_obj_set_style_text_align(label1, LV_TEXT_ALIGN_CENTER, 0);
            lv_obj_align(label1, LV_ALIGN_CENTER, 0, -40);

            lv_obj_t *label2 = lv_label_create(screens[i]);
            lv_label_set_long_mode(label2, LV_LABEL_LONG_SCROLL_CIRCULAR);     /*Circular scroll*/
            lv_obj_set_width(label2, 150);
            lv_label_set_text(label2, "It is a circularly scrolling # text. ");
            lv_obj_align(label2, LV_ALIGN_CENTER, 0, 40);
        } else if (i == 2) {
            dobj.timer = lv_label_create(screens[i]);
            lv_label_set_text(dobj.timer, "00:23");
            lv_obj_set_style_text_font(dobj.timer, &GIGA, 0);
            lv_obj_align(dobj.timer, LV_ALIGN_CENTER, 0, 0);
        } else {
            lv_obj_t* label = lv_label_create(screens[i]);
            char buf[20];
            snprintf(buf, sizeof(buf), "Screen %d", i + 1);
            lv_label_set_text(label, buf);
            lv_obj_center(label);
        }
    }
    lv_scr_load(screens[current_screen_index]);
}

void nextScreen() {
    current_screen_index = (current_screen_index + 1) % MAX_SCREENS;
    //lv_scr_load_anim(screens[current_screen_index], LV_SCR_LOAD_ANIM_MOVE_TOP, 200, 0, false);
    lv_scr_load(screens[current_screen_index]);
}

void prevScreen() {
    current_screen_index = (current_screen_index - 1 + MAX_SCREENS) % MAX_SCREENS;
    lv_scr_load(screens[current_screen_index]);
    //lv_scr_load_anim(screens[current_screen_index], LV_SCR_LOAD_ANIM_MOVE_BOTTOM, 200, 0, false);
}


void setupAmoled()
{
    /*
    * Compatible with touch version
    * Touch version, IO38 is the screen power enable
    * Non-touch version, IO38 is an onboard LED light
    * * */
    pinMode(PIN_LED, OUTPUT);
    digitalWrite(PIN_LED, LOW);

    rm67162_init(); // amoled lcd initialization

    lcd_setRotation(1);

    lv_init();

    buf = (lv_color_t *)ps_malloc(sizeof(lv_color_t) * LVGL_LCD_BUF_SIZE);
    assert(buf);


    lv_disp_draw_buf_init(&draw_buf, buf, NULL, LVGL_LCD_BUF_SIZE);

    /*Initialize the display*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    /*Change the following line to your display resolution*/
    disp_drv.hor_res = EXAMPLE_LCD_H_RES;
    disp_drv.ver_res = EXAMPLE_LCD_V_RES;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);
    
    init_screens();
    // show image
    
    #if 0


    #endif
}

void update_uptime_label(lv_obj_t* label) {
    // Assuming `esp_timer_get_time()` returns time in microseconds
    int64_t uptime_ms = esp_timer_get_time() / 1000; // Convert to milliseconds
    int seconds = (uptime_ms / 1000) % 60; // Convert milliseconds to seconds and find remainder over 60
    int minutes = (uptime_ms / (1000 * 60)) % 60; // Convert milliseconds to minutes and find remainder over 60

    char buffer[6]; // Buffer to hold the formatted string, "MM:SS"
    snprintf(buffer, sizeof(buffer), "%02d:%02d", minutes, seconds);

    lv_label_set_text(label, buffer);
}

void amoledLoop() {
    update_uptime_label(dobj.timer);
    lv_timer_handler();
}