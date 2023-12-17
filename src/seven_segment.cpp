#include <Arduino.h>
#include "seven_segment.h"

hw_timer_t * seven_segment_timer = NULL;
volatile uint8_t display[5];
int brightness = 0;


void displayDigit(int digit, int dot) {
  uint8_t segments = font[digit%10]; // Get the segments for the digit
  digitalWrite(seven_segment_pins[0], !(segments & 0b00000001));
  digitalWrite(seven_segment_pins[1], !(segments & 0b00000010));
  digitalWrite(seven_segment_pins[2], !(segments & 0b00000100));
  digitalWrite(seven_segment_pins[3], !(segments & 0b00001000));
  digitalWrite(seven_segment_pins[4], !(segments & 0b00010000));
  digitalWrite(seven_segment_pins[5], !(segments & 0b00100000));
  digitalWrite(seven_segment_pins[6], !(segments & 0b01000000));
  digitalWrite(seven_segment_pins[7], !dot);
}


void IRAM_ATTR onTimer() {
  static int cur_disp;
  // Turn off current display:
  digitalWrite(digits[cur_disp%4], LOW);
  cur_disp++;
  // Swap in new number
  displayDigit(display[cur_disp%4], display[4]);
  // Enable display in the new slot
  digitalWrite(digits[cur_disp%4], HIGH);
  if (brightness < 2) {
    delayMicroseconds(brightness);
    digitalWrite(digits[cur_disp%4], LOW);
  }
}

void show(uint8_t *disp) {
    // quick hack since display is volatile:
    display[0] = disp[0];
    display[1] = disp[1];
    display[2] = disp[2];
    display[3] = disp[3];
    display[4] = disp[4];
}


void setupDisplay() {
  /* Set all seven segment parts to output/high */
  for(int i = 0; i < ssp_len; i++) {
    digitalWrite(seven_segment_pins[i], HIGH);
    pinMode(seven_segment_pins[i], OUTPUT);
    digitalWrite(seven_segment_pins[i], HIGH);
  }
  
  /* Set all displays to output/low */
  for(int i = 0; i < d_len; i++) {
    digitalWrite(digits[i], LOW);
    pinMode(digits[i], OUTPUT);
    digitalWrite(digits[i], LOW);
  }

  /* Set up timer to cycle trough all digits */
  seven_segment_timer = timerBegin(0, 80, true); // Timer 0, prescaler 80, count up
  timerAttachInterrupt(seven_segment_timer, &onTimer, true);
  timerAlarmWrite(seven_segment_timer, 10, true); // 10 ticks (0.01 ms), auto reload true
  timerAlarmEnable(seven_segment_timer);
}