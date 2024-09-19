
// 
// MAIN PROGRAM
// licensed under CC-BY-NC 4.0
// 
// copyright (c) SkillIssue Team, Berlin, 2024
//

#include <Wire.h>
#include <thread>

#include "fadc.h"
#include "color.h"
#include "motor.h"
#include "shiftregister.h"
#include "lightsensor.h"
#include "linefollower.h"
#include "menu.h"
#include "gyro.h"
#include "claw.h"

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

    // init SR
    Serial.println("Shiftregister init...");
    shiftregister::setup();
    shiftregister::reset();

    Serial.println("Wire init...");
    Wire.begin(SDA, SCL); // start i2c
    Wire.setClock(400000); // Fast mode

    // setup FastADC
    //Serial.println("FADC begin...");
    //fadc::begin();
    
    // create lightSensor objects
    ls::setup();

    // run the led test
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
        // TODO: show that display init failed
    }

    Serial.println("PWM bus init");
    claw::setup();

    #ifdef CLAW_TEST
        claw::up();
        claw::down();
    #endif

    Serial.println("Checking Buttons for failures...");
    // if a button has failed (is pressed when he shouldn't)
    // we disable buttons by setting the button_failure flag
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
                attachInterrupt(T_E, isr, RISING); // make calibrating (soft-)abortable
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

                ls::save(); // save values to a json file
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
    //xTaskCreatePinnedToCore(core0_loop, "Core0MainLoop", 10000, NULL, 0, &loop0, 0);

    timestamp = micros();
}

void loop(){
    // we start a thread to control the motors (line follower)
    // using backed up values from the last light sensors reading
    // to avoid race conditions between the acces and the new values
    // due to reading the light values simultaneusly
    thread t(lf::follow);
        ls::read({&ls::white});
    t.join();
    ls::update({&ls::white}); // update the data with the new values

    color::update(); // update color detection
    gyro::update();  // update gyro
    if (!(digitalRead(T_L) || digitalRead(T_R)) && !button_failure){ // check for obstacle using the buttons // TODO: slow down using the TOF-sensors
        // avoid obstacle:
        // go back and turn by 45 deg (right)
        motor::stop();
        motor::rev(200);
        motor::gyro(-45);
        motor::fwd(300);
        // go fwd/turn until black is reached
        motor::fwd(motor::motor::A, V_STD);
        motor::fwd(motor::motor::B, 13);
        delay(400);
        do {
            ls::white.read();
            color::update({&color::black});
        } while (color::black() == Side::NONE);
        // turn left until one side is on Black
        motor::fwd(250);
        motor::gyro(-20);
        motor::turn(V_STD);
        do {
            ls::white.read();
            color::update({&color::black});
        } while (!(color::black() & Side::RIGHT));
    }
    /*if (color::green() != Side::NONE && millis() - last_green >= GREEN_TIMEOUT){
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
    }*/
    
}

//void core0_loop(void * pvargs){
//    while(true){
//        
//    }
//}
