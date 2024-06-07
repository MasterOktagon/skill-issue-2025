
// 
// MAIN PROGRAM
// licensed under CC-BY-NC 4.0
// 
// copyright (c) SkillIssue Team, Berlin, 2024
//

// multithreading includes
#include <mutex>
#include <thread>
#include <Wire.h>
#include <esp_task_wdt.h>

#include "motor.h"
#include "shiftregister.h"
#include "lightsensor.h"
#include "adc.h"
#include "linefollower.h"
#include "menu.h"
#include "color.h"

using namespace std;

bool button_failure = false; // wether buttons have a failure
TaskHandle_t loop0;

#ifdef RASP_COMM
    TwoWire comm(1); // I2C Bus for communcating with the Rasberry Pi
#endif

void IRAM_ATTR isr() {
    /*
    interruption service routine for reset
    */
	ESP.restart();
}
long timestamp;

// gets executed before loop
void setup(){
    // begin Serial connection (DEBUG)
    Serial.begin(115200);
    Serial.println("");
    Serial.println("Serial init [115200] ...");
    Serial.println("Shiftregister init...");
    shiftregister::setup();
    shiftregister::reset();

    Serial.println("ADC-Multi init...");
    adc::setup();

    #ifdef LED_TEST
        Serial.println("LED test (wgr[b])");
        shiftregister::set(SR_PT_WHITE, HIGH);
        delay(1500);
        shiftregister::set(SR_PT_WHITE, LOW);
        shiftregister::set(SR_PT_GREEN, HIGH);
        delay(1500);
        shiftregister::set(SR_PT_GREEN, LOW);
        shiftregister::set(SR_PT_RED, HIGH);
        delay(1500);
        shiftregister::set(SR_PT_RED, LOW);
    #endif

    // start SPIFFS
    fs::setup();

    Serial.println("Display init...");
    if (!menu::DisplayInit()){
        shiftregister::set(SR_LED_L_RED, LOW); // Debug display setup failed
    }
    pinMode(ENC_SW, INPUT);

    Serial.println("Checking Buttons for failures...");
    pinMode(T_L, INPUT);
    pinMode(T_R, INPUT);
    if (!(digitalRead(T_L) || digitalRead(T_R))){
        Serial.println("Button failure detected, disabling buttons!");
        menu::showWaiting("Button failure detected, disabling buttons!");
        button_failure = true;
    }
    #ifdef RASP_COMM
        Serial.println("Starting RPI communication...");
        comm.begin();     // Start master (transmitter)
        comm.begin(0x01); // Start slave (receiver)
    #endif

    // menu selection
    int selected = 0;
    while((selected = menu::menu()) != MENU_RUN){
        switch (selected){
            default:
                break;

            case MENU_CALIBRATE:
                delay(500);
                attachInterrupt(ENC_SW, isr, RISING);
                delay(1500);
                ls::calibrate(10000, 1);

                // Print min/max values
                Serial.print("white "); Serial.println(ls::white._str().c_str());
                Serial.print("green "); Serial.println(ls::green._str().c_str());
                Serial.print("red ");   Serial.println(ls::red._str().c_str());
                #ifdef LF_USE_BACK
                    #if (BOARD_REVISION > 1)
                        Serial.print("back ");   Serial.println(str(ls::back).c_str());
                    #endif
                #endif
                ls::save();
                detachInterrupt(ENC_SW);
        }
    }

    ls::load();

    Serial.print("white "); Serial.println(ls::white._str().c_str());
    Serial.print("green "); Serial.println(ls::green._str().c_str());
    Serial.print("red ");   Serial.println(ls::red._str().c_str());
    #ifdef LF_USE_BACK
        #if (BOARD_REVISION > 1)
            Serial.print("back ");   Serial.println(str(ls::back).c_str());
        #endif
    #endif

    delay(500); // delay to not interrupt directly
    attachInterrupt(ENC_SW, isr, RISING);

    // start i2c-Handler thread
    xTaskCreatePinnedToCore(core0_loop, "Core0MainLoop", 10000, NULL, 0, &loop0, 0);

    timestamp = micros();
}

void loop(){
    ls::read();
    color::update();
    if (!(digitalRead(T_L) || digitalRead(T_R)) && !button_failure){
        // avoid obstacle
        motor::stop();
        delay(2000);
    }
    if (color::green() != Side::NONE){
        // manage intersections
        motor::stop();
        delay(5000);
    }
    lf::follow();
    //Serial.print(ls::red.left.value);
    //Serial.print("\t");
    //Serial.print(ls::red.right.value);
    //Serial.print("\t");
    //Serial.println(lf::follow());
    //timestamp = micros();
}

void core0_loop(void * pvargs){
    while(true){
        
    }
}
