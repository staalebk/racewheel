#include <Arduino.h>
#include <Preferences.h>
#include "pitTimer.h"

uint8_t pitTimer[5];
struct timeval tv_begin = {0, 0};
int additional_time;

Preferences preferences;

void writeTimevalToNVS(struct timeval tv) {
    preferences.begin("time-storage", false);
    preferences.putULong("seconds", tv.tv_sec);
    preferences.end();
}

struct timeval readTimevalFromNVS() {
    struct timeval tv;
    preferences.begin("time-storage", true);
    tv.tv_sec = preferences.getULong("seconds", 0);
    tv.tv_usec = 0;
    preferences.end();
    additional_time = tv.tv_sec;
    return tv;
}


// Function to subtract two timeval structures
struct timeval timeval_subtract(struct timeval tv1, struct timeval tv2) {
    struct timeval result;

    // Subtract the microseconds
    result.tv_usec = tv1.tv_usec - tv2.tv_usec;

    // Subtract the seconds
    result.tv_sec = tv1.tv_sec - tv2.tv_sec;

    // Handle the case where microseconds subtraction goes below zero
    if (result.tv_usec < 0) {
        result.tv_usec += 1000000; // Add one second worth of microseconds
        result.tv_sec -= 1;        // Subtract one second
    }

    return result;
}

void pitTimerLoop() {
    struct timeval tv_now, tv_run;
    gettimeofday(&tv_now, NULL);

    // Check if the BOOT button is pressed
    /*
    if (digitalRead(0) == LOW) {
      if(bleKeyboard.isConnected() && ((tv_now.tv_sec - tv_begin.tv_sec) > 0)) {
        Serial.println("Sending start/stop");
        //bleKeyboard.write(KEY_MEDIA_PLAY_PAUSE);
        bleKeyboard.press(KEY_MEDIA_PLAY_PAUSE);
        delay(1200);
        bleKeyboard.release(KEY_MEDIA_PLAY_PAUSE);
      }
      tv_begin = tv_now;
      additional_time = 0;
    } else {
      tv_now.tv_sec += additional_time;
    }
    */
    tv_now.tv_sec += additional_time;
    // Calculate the run time
    tv_run = timeval_subtract(tv_now, tv_begin);

    // Convert to time structure
    struct tm *tm = localtime(&tv_run.tv_sec);

    // Extract hours, minutes, and seconds
    int hours = tm->tm_hour;
    int minutes = tm->tm_min;
    int seconds = tm->tm_sec;
    int hundredths = tv_run.tv_usec / 10000;  // Calculate hundredths of a second
    static int saved_time = 0;

    // Determine display based on elapsed time
    if (hours == 0) {
      if (minutes < 4) {
        // Save time every second the first 4 minutes, then only every minute
        if (seconds != saved_time)
          writeTimevalToNVS(tv_run);
        saved_time = seconds;
      } else {
        if (minutes != saved_time)
          writeTimevalToNVS(tv_run);
        saved_time = minutes;
      }
      if (minutes == 0) {
        // Less than one minute: display seconds and hundredths of a second
        pitTimer[0] = seconds / 10;
        pitTimer[1] = seconds % 10;
        pitTimer[2] = hundredths / 10;
        pitTimer[3] = hundredths % 10;
      } else {
        // Less than one hour: display minutes and seconds
        pitTimer[0] = minutes / 10;
        pitTimer[1] = minutes % 10;
        pitTimer[2] = seconds / 10;
        pitTimer[3] = seconds % 10;
      }
    } else {
      if (minutes != saved_time) {
        writeTimevalToNVS(tv_run);
        saved_time = minutes;
      }
      // More than one hour: display hours and minutes
      pitTimer[0] = hours / 10;
      pitTimer[1] = hours % 10;
      pitTimer[2] = minutes / 10;
      pitTimer[3] = minutes % 10;
    }

    // Handle dot blinking or other indications here
    if (hundredths < 10)
      pitTimer[4] = 1;
    else
      pitTimer[4] = 0;
}

void readPitTimer(uint8_t *disp) {
    memcpy(disp, pitTimer, 5);
}

int setupPitTimer() {
  return readTimevalFromNVS().tv_sec;
}

void pitIn() {
    struct timeval tv_now, tv_run;
    gettimeofday(&tv_now, NULL);
    Serial.println("PIT IN");
    // Calculate the run time
    tv_run = timeval_subtract(tv_now, tv_begin);

  //  if(tv_run.tv_sec > 60*4){
        tv_begin = tv_now;
        additional_time = 0;
  //  }
}

void pitOut() {
    struct timeval tv_now, tv_run;
    gettimeofday(&tv_now, NULL);
    Serial.println("PIT OUT");
    // Calculate the run time
    tv_run = timeval_subtract(tv_now, tv_begin);

  //  if(tv_run.tv_sec > 60*4){
        tv_begin = tv_now;
        additional_time = 0;
  //  }
}