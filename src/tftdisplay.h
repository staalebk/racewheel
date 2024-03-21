#pragma once
#include "ble.h"
#define LGFX_LOLIN_D32_PRO
#include "screen.h"
#include <LovyanGFX.hpp>
#include <LGFX_TFT_eSPI.hpp>
#include <TJpg_Decoder.h>
#include "SPIFFS.h"

#define SCREEN1 26
#define SCREEN2 25
TFT_eSPI tft = TFT_eSPI();

bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h,
				uint16_t *bitmap)
{
	if (y >= tft.height())
		return 0;
	tft.pushImage(x, y, w, h, bitmap);
	return 1;
}

void selectScreen(uint8_t screen)
{
	if (screen == SCREEN1)
	{
		digitalWrite(SCREEN1, 0); // turn on
		digitalWrite(SCREEN2, 1); // turn off
	}
	else if (screen == SCREEN2)
	{
		digitalWrite(SCREEN1, 1); // turn off
		digitalWrite(SCREEN2, 0); // turn on
	}
}

void screensOn()
{
	// all screens on
	digitalWrite(SCREEN1, 0); // turn on
	digitalWrite(SCREEN2, 0); // turn on
}

void screensOff()
{
	// all screens off
	digitalWrite(SCREEN1, 1); // turn off
	digitalWrite(SCREEN2, 1); // turn off
}

void clearScreens()
{
	screensOn();
	tft.fillScreen(TFT_BLACK);
}

void setupTFT() {
    Serial.println("Configuring FS...");
    SPIFFS.begin();
    Serial.println("Configuring screen(s)...");
    pinMode(SCREEN1, OUTPUT);
	pinMode(SCREEN2, OUTPUT);
	screensOn();

	// Initialize TFT display
	tft.init();
	tft.invertDisplay(1);
	tft.setRotation(3);
	tft.fillScreen(TFT_BLACK);
	tft.setSwapBytes(true);
    tft.setBrightness(128);
  
    char message[] = "Booting";
	tft.setTextSize(1);
	tft.setTextPadding(tft.textWidth(message, &fonts::Font4));
	tft.setTextColor(TFT_WHITE, TFT_BLACK);
	tft.setTextDatum(TC_DATUM);
    
    //tft.drawString(message, tft.width() / 2, 120, &fonts::Font4);
    // Display logo
    TJpgDec.setJpgScale(1);
	TJpgDec.setCallback(tft_output);
	TJpgDec.drawFsJpg(0, tft.height() / 2 - 45, "/timerdisplaylogo.jpg");
    

}