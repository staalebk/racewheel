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
//#include "tftdisplay.h"
#include "amoled.h"
#include "network.h"
#include "pins_config.h"
#include <Bounce2.h>
#define ONBOARD_LED 22

volatile bool dot;

const char *prefsNamespace = "PitTimer";
const char *counterKey = "time";
const char RCBleName[] = "RC DIY #ABCD";

PitState pitstate = Unknown;

//BleKeyboard bleKeyboard("Driftfun Race", "Driftfun", 100);
uint8_t disp[5];
Bounce2::Button next = Bounce2::Button(); // next screen button
Bounce2::Button prev = Bounce2::Button(); // next screen button
void setup() {
  Serial.begin(115200);
  
  //setupTFT();
  Serial.println("Starting...");
  setupAmoled();
  Serial.println("Displayinit done.");

  /* Set button as INPUT */
  //pinMode(0, INPUT_PULLUP);

  /* Configure onboard LED */
  //pinMode(ONBOARD_LED, OUTPUT);
  //digitalWrite(ONBOARD_LED, LOW);
  //setupDisplay();
  //int offset = setupPitTimer();  
  //Serial.printf("Timer should start on %d secs\n", offset);
  //Serial.println("Starting BLE work!");
  //bleKeyboard.begin();
  //setupRCBle(RCBleName);
  //setupNetwork();
  next.attach(PIN_BUTTON_1, INPUT_PULLUP);
  next.interval(5);
  next.setPressedState(LOW);
  prev.attach(PIN_BUTTON_2, INPUT_PULLUP);
  prev.interval(5);
  prev.setPressedState(LOW);

}


void loop() {
  Serial.println("loopdeloop");
  next.update();
  prev.update();
  if (next.pressed()) {
    nextScreen();
  }
  if (prev.pressed()) {
    prevScreen();
  }
#if 0
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
#endif
  //pitTimerLoop();
  //pollRCBle();
  //readPitTimer(disp);
  //show(disp);
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
  amoledLoop();
  delay(10);
}
