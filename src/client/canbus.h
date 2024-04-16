#pragma once

// Honda Accord 2006:
#define CAN_ID_VSA 0x64
// 0x64   5x in time period 
// 0xFF 0xC0 0x00 BBBB 0x0E 0x00 0x65 CHSUM
// BBBB: First nibble defines brake system indicators. Lower 4 bits define individual indicators
// bit0 VSA Activation
// bit1 VSA
// bit2 ABS
// bit3 BRAKE

#define CAN_ID_VSS  0xC8
// 0xC8  238x in time period 
// VSS VSS 0x00 0x00 VSS VSS ODO CHSUM
// VSS = Value = km/h * 100   Value = Speed sensor frequency * 3.623
// ODO = increments every .01 km.

#define CAN_ID_RPM 0x20C
// 0x20C  0x in time period 
// 0x02 MAINT 0x00 0x00 RPM RPM 0x00 CHSUM
// maint: 64 = 100%, 0 = 0%
// RPM: direct value

#define CAN_ID_ECT_RPM_MIL 0x12C
// 0x12C 239x in time period 
// ECT 0x46 0x00 0x00 RPM RPM MIL CHSUM
// ECT = value -40 (-40 to 215 celsius)
// RPM = direct value
// MIL = 40 - NO MIL. 50 - MIL ACTIVATED


// chsum:
// First four bits of the checksum is a a repeating pattern:
// 0, 4, 8, C and then repeat, in that order.
// Add up all payload 4-bits, including the first 4 bits of the checksum.
// substract that number from 0, and use the lowest 4 bits of the result as the final 4 bits of the checksum.
#define CAN_ID_MYSTERY_1 0x188
#define CAN_ID_MYSTERY_2 0x1C0
#define CAN_ID_MYSTERY_3 0x1F4  // Something with dash lights
#define CAN_ID_MYSTERY_4 0x224  // Something counting up (but not when rpms are falling)
#define CAN_ID_MYSTERY_5 0x2E7  // Something with state of stuff
#define CAN_ID_MYSTERY_6 0x327
#define CAN_ID_MYSTERY_7 0x40C
// 0x90    0       0       8       0x02    0x02    0xFF    0xF2    0x00    0x0F    0x00    0x0E
// 0x90    0       0       8       0x02    0x02    0xFF    0xF5    0x00    0x0F    0x00    0x47
// 0x90    0       0       8       0x02    0x02    0xFF    0xF8    0x00    0x0F    0x00    0x80
//                                                steering angle
#define CAN_ID_MYSTERY_8 0x90 // Steering angle, and more 466 is straight ahead, lower numbers are to the right
// 0xA6    0       0       8       0x20    0x02    0xD0    0xD0    0xC6    0x00    0x00    0x4C  All the way in
// 0xA6    0       0       8       0x20    0x02    0xCC    0xD0    0xC3    0x00    0x00    0x80
// 0xA6    0       0       8       0x20    0x02    0x8F    0xD0    0x85    0x00    0x00    0xCF
// 0xA6    0       0       8       0x20    0x02    0x47    0xD0    0x3E    0x00    0x00    0x03
// 0xA6    0       0       8       0x20    0x02    0x1F    0xD0    0x16    0x00    0x00    0x44
// 0xA6    0       0       8       0x20    0x02    0x0B    0xCE    0x02    0x00    0x00    0x8D
// 0xA6    0       0       8       0x20    0x02    0x09    0xC7    0x00    0x00    0x00    0xC4
// 0xA6    0       0       8       0x20    0x02    0x07    0x08    0x00    0x00    0x00    0xC1  All the way out
#define CAN_ID_MYSTERY_9 0xA6 // Throttle
#define CAN_ID_MYSTERY_10 0xD4
#define CAN_ID_MYSTERY_11 0x405

void setupCanbus();
void pollCanbus();