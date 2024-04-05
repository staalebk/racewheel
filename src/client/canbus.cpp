#include <Arduino.h>
#include "driver/twai.h"
#include "client.h"
#include "canbus.h"

void setupCanbus() {
    pinMode(CAN_RS, OUTPUT);    // INPUT (high impedance) = slope control mode, OUTPUT = see next line
    digitalWrite(CAN_RS, LOW);  // LOW = high speed mode, HIGH = low power mode (listen only)
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT((gpio_num_t)CAN_TX, (gpio_num_t)CAN_RX, TWAI_MODE_LISTEN_ONLY);
    twai_timing_config_t t_config  = TWAI_TIMING_CONFIG_500KBITS();
    twai_filter_config_t f_config  = TWAI_FILTER_CONFIG_ACCEPT_ALL();
    twai_driver_install(&g_config, &t_config, &f_config);
    twai_start();
}

void printCanMsg(twai_message_t *message) {
    Serial.print("0x");
    Serial.print(message->identifier, HEX);
    Serial.print("\t");
    Serial.print(message->extd);
    Serial.print("\t");
    Serial.print(message->rtr);
    Serial.print("\t");
    Serial.print(message->data_length_code);
    
    for(int i=0;i<message->data_length_code;i++) {
        Serial.print("\t0x");
        if (message->data[i]<=0x0F) {
            Serial.print(0);
        }
        Serial.print(message->data[i], HEX);
    }
    Serial.println();
}

void decodeVSA(twai_message_t *message) {
    static unsigned long lastPrintTime = 0;
    unsigned long currentTime = millis();

    uint8_t brake_system_indicators;
    bool vsa_active;
    bool vsa;
    bool abs;
    bool brake;

    if (message->data_length_code < 7)
        return;

    brake_system_indicators = message->data[3] >> 4;
    vsa_active = message->data[3] & 0b0001;
    vsa        = message->data[3] & 0b0010;
    abs        = message->data[3] & 0b0100;
    brake      = message->data[3] & 0b1000;

    // Check if less than 1000 milliseconds have passed since last print
    if (currentTime - lastPrintTime < 1000) {
        // It has not been a full second since the last print
        return;
    }

    // Update lastPrintTime with the current time after printing
    lastPrintTime = currentTime;

    Serial.print("Brake System Indicators: ");
    Serial.print(brake_system_indicators, HEX); // Printing in HEX as it's a bitmask
    Serial.print(", VSA Active: ");
    Serial.print(vsa_active ? "True" : "False");
    Serial.print(", VSA: ");
    Serial.print(vsa ? "True" : "False");
    Serial.print(", ABS: ");
    Serial.print(abs ? "True" : "False");
    Serial.print(", Brake: ");
    Serial.println(brake ? "True" : "False"); // Use Serial.println() for the last part to add a newline
}

void decodeVSS(twai_message_t *message) {
    static unsigned long lastPrintTime = 0;
    unsigned long currentTime = millis();

    uint16_t vss1;
    uint16_t vss2;
    uint8_t odo;

    if (message->data_length_code < 7)
        return;

    vss1  = message->data[0] << 8;
    vss1 |= message->data[1];

    vss2  = message->data[4] << 8;
    vss2 |= message->data[5];

    odo = message->data[6];

    // Check if less than 1000 milliseconds have passed since last print
    if (currentTime - lastPrintTime < 1000) {
        // It has not been a full second since the last print
        return;
    }

    // Update lastPrintTime with the current time after printing
    lastPrintTime = currentTime;

    Serial.print("VSS1: ");
    Serial.print(vss1);
    Serial.print(", VSS2: ");
    Serial.print(vss1);
    Serial.print(", odo: ");
    Serial.println(odo);
}

void decodeRPM(twai_message_t *message) {
    static unsigned long lastPrintTime = 0;
    unsigned long currentTime = millis();

    uint16_t rpm;
    uint8_t maint;

    if (message->data_length_code < 7)
        return;

    maint  = message->data[1];

    rpm  = message->data[4] << 8;
    rpm |= message->data[5];

    // Check if less than 1000 milliseconds have passed since last print
    if (currentTime - lastPrintTime < 1000) {
        // It has not been a full second since the last print
        return;
    }

    // Update lastPrintTime with the current time after printing
    lastPrintTime = currentTime;

    Serial.print("maint: ");
    Serial.print(maint, HEX);
    Serial.print(", RPM: ");
    Serial.println(rpm);
}

void decodeECTRPMMIL(twai_message_t *message) {
    static unsigned long lastPrintTime = 0;
    unsigned long currentTime = millis();

    uint16_t rpm;
    int16_t ect;
    uint8_t mil;

    if (message->data_length_code < 7)
        return;

    ect  = message->data[0] - 40;

    rpm  = message->data[4] << 8;
    rpm |= message->data[5];

    mil  = message->data[6];

    // Check if less than 1000 milliseconds have passed since last print
    if (currentTime - lastPrintTime < 1000) {
        // It has not been a full second since the last print
        return;
    }

    // Update lastPrintTime with the current time after printing
    lastPrintTime = currentTime;

    Serial.print("ECT: ");
    Serial.print(ect);
    Serial.print(", RPM: ");
    Serial.print(rpm);
    Serial.print(", MIL: ");
    Serial.println(mil, HEX);
}

void pollCanbus() {
    twai_message_t message;
  
    if (twai_receive(&message, pdMS_TO_TICKS(1)) == ESP_OK) {
        switch(message.identifier) {
            case CAN_ID_VSA:
                decodeVSA(&message);
                break;
            case CAN_ID_VSS:
                decodeVSS(&message);
                break;
            case CAN_ID_RPM:
                decodeRPM(&message);
                break;
            case CAN_ID_ECT_RPM_MIL:
                decodeECTRPMMIL(&message);
                break;
            default:
                printCanMsg(&message);
        }
    }
}