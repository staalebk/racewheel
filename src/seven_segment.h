#pragma once
#include <Arduino.h>
#include "config.h"

/*
 Panel is 4x 7 segments, L1 - L4:
 L2 has the DP leds connected.
 The pinout of the panel is:

 12 11 10 09 08 07 |
 L1  A  F L2 L3  B |
                   | 
  E  D DP  C  G L4 |
 01 02 03 04 05 06 | 

Each 7 segment is arranged like this:

 -- A --
|       |
F       B  DP
|       |
 -- G --
|       |
E       C  DP
|       |
 -- D --
*/

static const uint8_t font[] = {
   //GFEDCBA
  0b00111111, // 0
  0b00000110, // 1
  0b01011011, // 2
  0b01001111, // 3
  0b01100110, // 4
  0b01101101, // 5
  0b01111101, // 6
  0b00000111, // 7
  0b01111111, // 8
  0b01101111  // 9
};

static const uint8_t ssp_len = sizeof(seven_segment_pins)/sizeof(seven_segment_pins[0]);
static const uint8_t d_len = sizeof(digits)/sizeof(digits[0]);

void show(uint8_t *disp);
void setupDisplay();