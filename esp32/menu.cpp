

//
// MENU.cpp
// 
// this module contains high-level functions that do output on the Display [W.I.P.]
//


// No idea why arduino added this
#include <esp32-hal-gpio.h>
#include <esp32-hal-adc.h>
#include <SPI.h> // comms
#include <Wire.h>
#include <cmath>
#include "FreeRTOS.h"

#include "Pins.h" // pin layout
#include "shared.h"
#include "Arduino.h"
#include "menu.h"
#include "fadc.h"

#include <Adafruit_GFX.h> //https://github.com/adafruit/Adafruit-GFX-Library
#include <Adafruit_SSD1306.h> //https://github.com/adafruit/Adafruit_SSD1306

#include "icons.h" // icons to be displayed
#include "shiftregister.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)

#define SHOW_VBAT
#define VIEW_START 26 // We only have 128 pixels on the screen, so we only display from 26 to 154
#define VIEW_END 154

namespace menu {

    Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

    bool DisplayInit(){
    if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADRESS)) {
        output.println("SSD1306 allocation failed!");
            return false;
        }
        display.display();
        display.clearDisplay();
        display.drawBitmap(0,0,logo_Bitmap, 128, 64, SSD1306_WHITE);
        display.display();
        delay(1500);

        #ifdef SHOW_VBAT
            pinMode(VBAT, INPUT);
            analogReadResolution(12);
        #endif

        pinMode(T_E, INPUT_PULLUP);

        return true;
    }

    void overlay(){
        #ifdef SHOW_VBAT
            float BATVoltage = (float)analogRead(VBAT) / 4096 * 4.8 * 5.1 / 2.2;
            unsigned int BATPercent = (float)(BATVoltage - 7) * 100 / 1.4 + 0;
            if (BATPercent > 100) {
                BATPercent = 100;
                if (BATVoltage < 7.00){
                    BATPercent = 0;
                }
            }
            display.setCursor(0, 0);
            //display.print(String((1649/649*analogRead(VBAT))/1000));
            display.print(String(BATPercent));
            display.print("%");
        #endif
    }

    void showWaiting(const char* msg){
        display.clearDisplay();
        overlay();
        display.setCursor(0,16);
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);
        display.println(msg);
        display.display();
    }


    #define menuOptions 3

    int menu(bool button_failure){
        int selected = 0;
        const unsigned char * icons[menuOptions] = {iconRun, iconCalibrate, iconLog}; // icons to display in menu
        const char * texts[menuOptions] = {"Run", "Calibrate", "Print Log"}; // text of the options

        bool in_menu = true;
        while (in_menu && !button_failure){
            display.clearDisplay();
            display.setTextColor(SSD1306_WHITE);
            overlay();
            for(int i = 0; i < menuOptions; i++){ // draw icons
                if (i == selected){ // highlight selected
                display.fillRoundRect(32*i, 16, 32, 32, 4, SSD1306_WHITE);
                }
                display.drawBitmap(32*i, 16, icons[i], 32, 32, SSD1306_INVERSE);
            }
            display.setCursor(8, 49);
            display.print(texts[selected]);

            display.display();
            if (digitalRead(T_E) == LOW){ // -> clicked
                in_menu = false; // -> break while loop
                return selected;
            }
            if (!button_failure){
                selected = (selected + digitalRead(T_L) - digitalRead(T_R)) % menuOptions;
                if (selected < 0){selected = menuOptions-1;}
                // output.print("\t"); output.println(texts[selected]);
                delay(150);
            }
            delay(10);
        }
        // => button failure
        delay(2000);
        return MENU_RUN;
    }


    void showDifference(int16_t value, bool clear=false){
        overlay();
        if (clear){
            display.clearDisplay();
            display.drawFastVLine(64, 32-8, 16, SSD1306_WHITE);
            //display.setTextSize(2);
            display.setTextColor(SSD1306_WHITE);
        }
        int16_t xShift = value;
        display.drawFastVLine(xShift + 64, 32-8, 16, SSD1306_WHITE);
        //display.drawFastHLine(xShift, 64, 32, uint16_t color);
        display.display();
    }

    /*void showRotation(){
        overlay();
        display.clearDisplay();
        display.drawCircle(64,32, 8, SSD1306_WHITE);
        display.drawFastVLine(64, 15, 17, SSD1306_WHITE);
        display.drawLine(64, 32, round(64 + 9 * cos(gyro::ZAngle)), round(32 + 9 * sin(gyro::ZAngle)), SSD1306_WHITE);

        display.display();
    }*/
}
