#pragma once

/*
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
// Pin assignment for segments               A   B   C   D   E   F   G   DP
static const uint8_t seven_segment_pins[] = {16, 23, 25, 27, 14, 17, 33, 26};
// Pin assignment for each of the four 7-segments
static const uint8_t digits[] = {4, 5, 18, 32};

// Pin assignment for the buttons
static const uint8_t buttons[] = {15, 3, 22, 21, 19};