#pragma once
#include<Arduino.h>

/*
This needs to be called often, at least once a second, preferably at least once every 10ms
The main loop is a good place to call this
*/
void pitTimerLoop();
void setupPitTimer();
void readPitTimer(uint8_t *disp);