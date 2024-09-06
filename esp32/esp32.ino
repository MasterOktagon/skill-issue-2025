
// 
// MAIN PROGRAM
// licensed under CC-BY-NC 4.0
// 
// copyright (c) SkillIssue Team, Berlin, 2024
//

#include <Wire.h>

#include "color.h"
#include "motor.h"
#include "shiftregister.h"
#include "lightsensor.h"
#include "linefollower.h"
#include "menu.h"
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
    Wire.begin(SDA, SCL); // start i2c
    Wire.setClock(400000); // Fast mode
    
    ls::setup();

    #ifdef LED_TEST
        Serial.println("LED test (wgr[b])");
        digitalWrite(PT_WHITE_L, HIGH);
        delay(1500);
        digitalWrite(PT_WHITE_L, LOW);
        digitalWrite(PT_WHITE_R, HIGH);
        delay(1500);
        digitalWrite(PT_WHITE_R, LOW);

        //TODO: make the rgb led test
    #endif

    // start SPIFFS
    fs::setup();

    Serial.println("Initializing gyro...");
    gyro::init();

    Serial.println("Display init...");
    if (!menu::DisplayInit()){
        ls::led.setPixelColor(0, ls::led.Color(255,0,0));
        ls::led.show();
    }

    Serial.println("Checking Buttons for failures...");
    pinMode(T_L, INPUT_PULLUP);
    pinMode(T_R, INPUT_PULLUP);
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
                attachInterrupt(T_E, isr, RISING);
                delay(1500);
                ls::calibrate(10000, 1);

                // Print min/max values
                Serial.print("white "); Serial.println(ls::white._str().c_str());
                Serial.print("green "); Serial.println(ls::green._str().c_str());
                Serial.println("red "); Serial.println(ls::red._str().c_str());
                Serial.println("");
                Serial.print("white_b "); Serial.println(ls::white_b._str().c_str());
                Serial.print("green_b "); Serial.println(ls::green_b._str().c_str());
                Serial.println("red_b "); Serial.println(ls::red_b._str().c_str());

                ls::save();
                detachInterrupt(T_E);
        }
    }

    ls::load();

    // Print min/max values
    Serial.print("white "); Serial.println(ls::white._str().c_str());
    Serial.print("green "); Serial.println(ls::green._str().c_str());
    Serial.println("red "); Serial.println(ls::red._str().c_str());
    Serial.println("");
    Serial.print("white_b "); Serial.println(ls::white_b._str().c_str());
    Serial.print("green_b "); Serial.println(ls::green_b._str().c_str());
    Serial.println("red_b "); Serial.println(ls::red_b._str().c_str());


    delay(500); // delay to not interrupt directly
    attachInterrupt(T_E, isr, RISING);

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
        ls::led.setPixelColor(0, ls::led.Color(0, 255 * int(!left),  255 * int(!left_black)));
        ls::led.setPixelColor(1, ls::led.Color(0, 255 * int(!right), 255 * int(!right_black)));
        ls::led.show();
        
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
        ls::led.clear();
        ls::led.show();
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
