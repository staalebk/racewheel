#include <Arduino.h>
#include <Preferences.h>
#include <BleKeyboard.h>

#define ONBOARD_LED 2

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
//                                           A   B   C   D   E   F   G   DP
static const uint8_t seven_segment_pins[] = {16, 23, 25, 27, 14, 17, 33, 26};
static const uint8_t ssp_len = sizeof(seven_segment_pins)/sizeof(seven_segment_pins[0]);
static const uint8_t digits[] = {4, 5, 18, 32};
static const uint8_t d_len = sizeof(digits)/sizeof(digits[0]);
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

hw_timer_t * timer = NULL;
volatile uint8_t display[4];
volatile bool dot;

Preferences preferences;
const char *prefsNamespace = "PitTimer";
const char *counterKey = "time";
struct timeval tv_begin;
int additional_time;

BleKeyboard bleKeyboard("Driftfun Race", "Driftfun", 100);


void inline displayDigit(int digit) {
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
  displayDigit(display[cur_disp%4]);
  // Enable display in the new slot
  digitalWrite(digits[cur_disp%4], HIGH);

  
}

void writeTimevalToNVS(struct timeval tv) {
    preferences.begin("time-storage", false);
    preferences.putULong("seconds", tv.tv_sec);
    preferences.end();
    int currentState = digitalRead(ONBOARD_LED); // Read the current state of the pin
    //digitalWrite(ONBOARD_LED, !currentState);    // Set the pin to the opposite state

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

void setup() {
  /* Set button as INPUT */
  pinMode(0, INPUT_PULLUP);

  /* Configure onboard LED */
  pinMode(ONBOARD_LED, OUTPUT);
  digitalWrite(ONBOARD_LED, LOW);

  /* Set all seven segment parts to output/high */
  for(int i = 0; i < ssp_len; i++) {
    digitalWrite(seven_segment_pins[i], HIGH);
    pinMode(seven_segment_pins[i], OUTPUT);
    digitalWrite(seven_segment_pins[i], HIGH);
  }

  readTimevalFromNVS();

  /* Set all displays to output/low */
  for(int i = 0; i < d_len; i++) {
    digitalWrite(digits[i], LOW);
    pinMode(digits[i], OUTPUT);
    digitalWrite(digits[i], LOW);
  }
  timer = timerBegin(0, 80, true); // Timer 0, prescaler 80, count up
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 10, true); // 10 ticks (0.01 ms), auto reload true
  timerAlarmEnable(timer);
  Serial.begin(115200);
  Serial.println("Starting BLE work!");
  bleKeyboard.begin();
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



void loop() {
    struct timeval tv_now, tv_run;
    static struct timeval tv_begin = {0, 0};  // Initialize tv_begin to zero
    gettimeofday(&tv_now, NULL);

    // Check if the BOOT button is pressed
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
        display[0] = seconds / 10;
        display[1] = seconds % 10;
        display[2] = hundredths / 10;
        display[3] = hundredths % 10;
      } else {
        // Less than one hour: display minutes and seconds
        display[0] = minutes / 10;
        display[1] = minutes % 10;
        display[2] = seconds / 10;
        display[3] = seconds % 10;
      }
    } else {
      if (minutes != saved_time) {
        writeTimevalToNVS(tv_run);
        saved_time = minutes;
      }
      // More than one hour: display hours and minutes
      display[0] = hours / 10;
      display[1] = hours % 10;
      display[2] = minutes / 10;
      display[3] = minutes % 10;
    }

    // Handle dot blinking or other indications here
    if (hundredths < 10)
      dot = true;
    else
      dot = false;
}
