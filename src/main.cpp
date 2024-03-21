#include "SPI.h"
#include <esp_system.h>
#include <esp_random.h>
#include <FS.h>
#include <Wire.h>
#include <Arduino.h>
#include <BleKeyboard.h>
#include <TJpg_Decoder.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "SPIFFS.h"
#include "driver/periph_ctrl.h"
#include "config.h"
#include "seven_segment.h"
#include "pitTimer.h"
#include "buttons.h"
#include "pit.h"
#include "ble.h"
#include "pos.h"
#include "tftdisplay.h"
#include "network.h"
#define ONBOARD_LED 22

volatile bool dot;

const char *prefsNamespace = "PitTimer";
const char *counterKey = "time";
const char RCBleName[] = "RC DIY #ABCD";

PitState pitstate = Unknown;

//BleKeyboard bleKeyboard("Driftfun Race", "Driftfun", 100);
uint8_t disp[5];

void setup() {
  Serial.begin(115200);
  
  setupTFT();

  /* Set button as INPUT */
  pinMode(0, INPUT_PULLUP);

  /* Configure onboard LED */
  pinMode(ONBOARD_LED, OUTPUT);
  digitalWrite(ONBOARD_LED, LOW);
  //setupDisplay();
  int offset = setupPitTimer();  
  Serial.printf("Timer should start on %d secs\n", offset);
  Serial.println("Starting BLE work!");
  //bleKeyboard.begin();
  setupRCBle(RCBleName);
  setupNetwork();
}


void loop() {
  if (isPushed(BUTTON_TOP_LEFT)) {
    Serial.println("LEFT");
 //   if(bleKeyboard.isConnected()) {
      Serial.println("ALT + TAB");
      //bleKeyboard.write(KEY_MEDIA_PLAY_PAUSE);
      //bleKeyboard.press(KEY_LEFT_ALT);
      //bleKeyboard.press(KEY_TAB);
      delay(100);
      //bleKeyboard.releaseAll();
 //   }
  }
  if (isPushed(BUTTON_TOP_RIGHT)) {
    Serial.println("RIGHT");
  //  if(bleKeyboard.isConnected()) {
      Serial.println("Sending start/stop");
      //bleKeyboard.write(KEY_MEDIA_PLAY_PAUSE);
      //bleKeyboard.press(KEY_MEDIA_PLAY_PAUSE);
      delay(1200);
      //bleKeyboard.release(KEY_MEDIA_PLAY_PAUSE);
   // }
  }
  
  pitTimerLoop();
  pollRCBle();
  readPitTimer(disp);
  show(disp);
  static int num = 0;
  if (!(num++ % 100)) {
    if (!hasValidPos()) {
      Serial.println("Waiting for valid position.");
      pitstate = Unknown;
    } else if (isInPit()) {
      if (pitstate == NotInThePits) {
        pitIn();
      }
      pitstate = InThePits;
    } else {
      if (pitstate == InThePits) {
        pitOut();
      }
      pitstate = NotInThePits;
    }
  }
  /*
  for(int i = 0; i < BUTTON_MAX; i++) {
    if(isPushed(i))
      Serial.println(buttons[i]);
  }
  */
  delay(10);
}
