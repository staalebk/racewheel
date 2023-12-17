#include<Arduino.h>
#include "buttons.h"
#include "config.h"

bool buttonPushed[BUTTON_MAX];

void setupButtons(){
    for(int i = 0; i < BUTTON_MAX; i++) {
        /* Set button as INPUT */
        pinMode(buttons[i], INPUT_PULLUP);
    }
}

bool isPushed(int button) {
    if(digitalRead(buttons[button]) == LOW) {
        // Only trigger once per push.
        if (!buttonPushed[button]) {
            buttonPushed[button] = true;
            return true;
        }
    } else {
        buttonPushed[button] = false;
    }
    return false;
}