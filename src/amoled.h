#pragma once
#include "lvgl.h"

typedef struct {
    lv_obj_t *timer;
} objects;

void setupAmoled();
void amoledLoop();
void nextScreen();
void prevScreen();