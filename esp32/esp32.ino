
// 
// MAIN PROGRAM
// licensed under CC-BY-NC 4.0
// 
// copyright (c) SkillIssue Team, Berlin, 2024
//

#include <Wire.h>

#include "motor.h"
#include "shiftregister.h"
#include "lightsensor.h"
#include "adc.h"
#include "linefollower.h"
#include "menu.h"
#include "color.h"
#include "gyro.h"

using namespace std;

#define GREEN_TIMEOUT 100

bool button_failure = false; // wether buttons have a failure
TaskHandle_t loop0; // Task handler for second loop
uint16_t last_green = 0;

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

    Serial.println("Wire init...");
    Wire.begin();
    Wire.setClock(400000); // Fast mode

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

    Serial.println("Initializing gyro...");
    gyro::init();

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
        delay(1000);
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
    gyro::update();
    if (!(digitalRead(T_L) || digitalRead(T_R)) && !button_failure){
        // avoid obstacle
        motor::stop();
        motor::rev(200);
        motor::gyro(-45);
        motor::fwd(300);
        motor::fwd(motor::motor::A, V_STD);
        motor::fwd(motor::motor::B, 13);
        delay(400);
        uint32_t t = millis(); // time passed
        do {
            ls::white.read();
            color::update({&color::black});
        } while (color::black() == Side::NONE);
        motor::fwd(250);
        motor::gyro(-20);
        motor::turn(V_STD);
        do {
            ls::white.read();
            color::update({&color::black});
        } while (!(color::black() & Side::RIGHT));
    }
    if (color::green() != Side::NONE && millis() - last_green >= GREEN_TIMEOUT){
        motor::fwd(motor::motor::AB, 70);
        #ifdef DEBUG
            Serial.println("Green Detected!");
        #endif
        // confirm the read values
        for(uint8_t i = 0; i < 15; i++){
            ls::read();
            color::update({&color::green, &color::black});
        }
        bool left  = color::green() & Side::LEFT;
        bool right = color::green() & Side::RIGHT;
        motor::fwd(motor::motor::AB, V_STD);
        // go fwd until there is no green
        while (color::green() != Side::NONE){
            ls::read();
            color::update();
        }
        // check for black line
        motor::read_fwd(V_STD, 50, {&color::black});
        bool left_black = color::black() & Side::LEFT;
        bool right_black = color::black() & Side::RIGHT;
        
        // debug green detection
        shiftregister::set(SR_LED_L_GREEN, !left, false);
        shiftregister::set(SR_LED_L_BLUE, !left_black, false);
        shiftregister::set(SR_LED_R_BLUE, !right_black, false);
        shiftregister::set(SR_LED_R_GREEN, !right);
        
        int16_t deg = 0; // how much to turn
        deg += 90 * int(left && left_black);
        deg += 90 * int(right && right_black);

        if (left && !(right)){
            deg *= -1;
        }
        color::green.reset();
        color::black.reset();
        motor::fwd(120);
        motor::gyro(deg);
        motor::fwd(60);
        // set freeze
        last_green = millis();

        // reset LEDs
        shiftregister::set(SR_LED_L_GREEN, HIGH, false);
        shiftregister::set(SR_LED_L_BLUE, HIGH, false);
        shiftregister::set(SR_LED_R_BLUE, HIGH, false);
        shiftregister::set(SR_LED_R_GREEN, HIGH);
    }
    if (color::red() != Side::NONE){
        motor::stop();
        delay(6000);
    }
    lf::follow();
}

void core0_loop(void * pvargs){
    while(true){
        
    }
}
