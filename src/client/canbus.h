#pragma once

// Honda Accord 2006:
#define CAN_ID_VSA 0x64
// 0x64
// 0xFF 0xC0 0x00 BBBB 0x0E 0x00 0x65 CHSUM
// BBBB: First nibble defines brake system indicators. Lower 4 bits define individual indicators
// bit0 VSA Activation
// bit1 VSA
// bit2 ABS
// bit3 BRAKE

#define CAN_ID_VSS  0xC8
// 0xC8
// VSS VSS 0x00 0x00 VSS VSS ODO CHSUM
// VSS = Value = km/h * 100   Value = Speed sensor frequency * 3.623
// ODO = increments every .01 km.

#define CAN_ID_RPM 0x20C
// 0x20C
// 0x02 MAINT 0x00 0x00 RPM RPM 0x00 CHSUM
// maint: 64 = 100%, 0 = 0%
// RPM: direct value

#define CAN_ID_ECT_RPM_MIL 0x12C
// 0x12C
// ECT 0x46 0x00 0x00 RPM RPM MIL CHSUM
// ECT = value -40 (-40 to 215 celsius)
// RPM = direct value
// MIL = 40 - NO MIL. 50 - MIL ACTIVATED


// chsum:
// First four bits of the checksum is a a repeating pattern:
// 0, 4, 8, C and then repeat, in that order.
// Add up all payload 4-bits, including the first 4 bits of the checksum.
// substract that number from 0, and use the lowest 4 bits of the result as the final 4 bits of the checksum.


void setupCanbus();
void pollCanbus();