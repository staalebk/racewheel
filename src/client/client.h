#pragma once
// Pinout v3.2

#define FORCE_ON    17
#define SENSE_V_DIG 8
#define SENSE_V_ANA 9

#define RED_LED     10
#define YELLOW_LED  11

#define CAN_RX      13
#define CAN_TX      14
#define CAN_RS      38    // Standby and/or faster rise times (from v3.2 only)

#define HI_DRIVER   21    // 3V3 high driver power output

#define SDA         1     // I2C 
#define SCL         2     // I2C

#define MISO        41    // SPI
#define MOSI        40    // SPI
#define CLK         39    // SPI
#define SD_CARD     45    // SPI CS for SD card reader
