
// 
// MAIN PROGRAM
// licensed under CC-BY-NC 4.0
// 
// copyright (c) SkillIssue Team, Berlin, 2024
//

#include <Wire.h>
#include <thread>
#include <exception>

#include "fadc.h"
#include "color.h"
#include "motor.h"
#include "shiftregister.h"
#include "lightsensor.h"
#include "linefollower.h"
#include "menu.h"
#include "gyro.h"
#include "claw.h"
#include "tof.h"
#include "rpi_com.h"

using namespace std;

#define GREEN_TIMEOUT 100

bool button_failure = false; // wether buttons have a failure
uint16_t last_green = 0;

long timestamp;

void IRAM_ATTR isr(){
    esp_restart();
}

void cal_movement(){
    unsigned long timestamp = millis();
    while(millis() - timestamp < 10000){
        int v = ((millis() - timestamp) % 3000) >= 1500 ? -70 : 70;
        output.println(v);
        motor::fwd(motor::motor::AB, v);
        delay(1);
    }
    motor::stop();
}

// gets executed before loop
void setup(){
    // begin output connection (DEBUG)
    Serial.begin(115200);
    //#ifndef DEBUG
    //    output = createWriter("out.log"); // log Serial output to file
    //#endif

    output.println("");
    output.println("output init [115200] ...");

    // init SR
    output.println("Shiftregister init...");
    shiftregister::setup();
    shiftregister::reset();
    motor::stop();

    output.println("Wire init...");
    Wire.begin(SDA, SCL);  // start i2c
    Wire.setClock(400000); // Fast mode

    // setup FastADC
    #ifdef FASTREAD
        output.println("FADC begin...");
        fadc::begin();
    #endif
    
    // create lightSensor objects
    ls::setup();

    output.println("Display init...");
    if (!menu::DisplayInit()){
        rgb::setValue(Side::BOTH, 255, 0, 0);
    }

    tof::setup();

    // run the led test
    #ifdef LED_TEST
        output.println("LED test (wwgr)");
        digitalWrite(PT_WHITE_L, HIGH);
        delay(1500);
        digitalWrite(PT_WHITE_L, LOW);
        digitalWrite(PT_WHITE_R, HIGH);
        delay(1500);
        digitalWrite(PT_WHITE_R, LOW);
        digitalWrite(PT_GREEN, HIGH);
        delay(1500);
        digitalWrite(PT_GREEN, LOW);
        digitalWrite(PT_RED, HIGH);
        delay(1500);
        digitalWrite(PT_RED, LOW);
    #endif

    // start SPIFFS
    fs::setup();

    output.println("Initializing gyro...");
    gyro::init();

    output.println("PWM bus init");
    claw::setup();
    rgb::setValue(Side::BOTH, 0, 0, 0);

    #ifdef CLAW_TEST
        output.println("Claw Test...");
        output.println("Open");
        claw::open();
        output.println("Close");
        claw::close();
        output.println("Wide");
        claw::wide();
        output.println("Close");
        claw::close();
    #endif

    #ifdef MOT_STBY
        shiftregister::set(SR_STBY1, LOW);
        shiftregister::set(SR_STBY2, LOW);
    #endif

    #ifdef MOT_TEST
        motor::fwd(1000);
        motor::rev(1000);
    #endif

    output.println("Checking Buttons for failures...");
    // if a button has failed (is pressed when it shouldn't be)
    // we disable buttons by setting the button_failure flag
    pinMode(T_L, INPUT_PULLUP);
    pinMode(T_R, INPUT_PULLUP);
    if (!(digitalRead(T_L) && digitalRead(T_R))){
        output.println("Button failure detected, disabling buttons!");
        menu::showWaiting("Button failure detected, disabling buttons!");
        rgb::setValue(Side::BOTH, 255, 0, 0);
        delay(1000);
        button_failure = true;
    }
    //output.println(rpi::status());

    
    // menu selection
    output.println("Menu");

    digitalWrite(PT_GREEN, HIGH);
    int selected = 0;
    while((selected = menu::menu(button_failure)) != MENU_RUN){
        switch (selected){
            default:
                break;

            case MENU_CALIBRATE:

                menu::showWaiting("Calibrating...");
                delay(1500);

                attachInterrupt(T_E, isr, RISING);
                thread t(cal_movement);
                    ls::calibrate(10000, 0);
                t.join();
                detachInterrupt(T_E);

                // Print min/max values
                output.print("white "); output.println(ls::white._str().c_str());
                output.print("green "); output.println(ls::green._str().c_str());
                output.println("red "); output.println(ls::red._str().c_str());
                output.println("");
                output.print("white_b "); output.println(ls::white_b._str().c_str());
                output.print("green_b "); output.println(ls::green_b._str().c_str());
                output.println("red_b "); output.println(ls::red_b._str().c_str());

                ls::save(); // save values to a json file
        }
    }

    ls::load();

    // Print min/max values
    output.print("white "); output.println(ls::white._str().c_str());
    output.print("green "); output.println(ls::green._str().c_str());
    output.println("red "); output.println(ls::red._str().c_str());
    output.println("");
    output.print("white_b "); output.println(ls::white_b._str().c_str());
    output.print("green_b "); output.println(ls::green_b._str().c_str());
    output.println("red_b "); output.println(ls::red_b._str().c_str());

    timestamp = micros();
    delay(2000);
    attachInterrupt(T_E, isr, RISING);
}

void loop(){
    // we start a thread to control the motors (line follower)
    // using backed up values from the last light sensors reading
    // to avoid race conditions between the acces and the new values
    // due to reading the light values simultaneusly
    //thread t(lf::follow);
    //    ls::read(false);
    //t.join();
    ls::read();
    lf::follow();
    delayMicroseconds(50);
    ls::update(); // update the data with the new values outside the thread

    color::update(); // update color detection
    gyro::update();  // update gyro
    
    if (!(digitalRead(T_L) && digitalRead(T_R)) && !button_failure){ // check for obstacle using the buttons // TODO: slow down using the TOF-sensors
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

    // We detect green using the difference between 
    // red and green light values
    // there is also a timeout since the last green green to
    // avoid detecting the same crossing two times
    if (color::green() != Side::NONE && millis() - last_green >= GREEN_TIMEOUT){
        rgb::setValue(color::green(), 0, 255, 0);
        Side l = color::green();
        #ifdef DEBUG
            output.println("Green Detected!");
        #endif

        // confirm the read values by reading 50ms more
        // (note we are still moving forwards. this helps
        // detecting double points when approaching unaligned)
        motor::read_fwd(70, 100, {&color::green}); // reduce motor speed to allow better read resolution
        bool left  = (color::green() & Side::LEFT)  || l & Side::LEFT;
        bool right = (color::green() & Side::RIGHT) || l & Side::RIGHT;

        motor::fwd(motor::motor::AB, V_STD);
        // go fwd until there is no green
        while (color::green() != Side::NONE){
            ls::read();
            color::update();
        }
        motor::stop(true);

        // check for black line
        motor::read_fwd(V_STD, 50, {&color::black});
        bool left_black  = color::black() & Side::LEFT;
        bool right_black = color::black() & Side::RIGHT;
        
        // debug green detection on the onboard RGB LEDs
        rgb::setValue(Side::RIGHT,  0, left*255,  !left_black*255);
        rgb::setValue(Side::LEFT, 0, right*255, !right_black*255);

        motor::stop();
        delay(1000);
        
        int16_t deg = 0; // how much to turn
        deg += 90 * int(left && left_black);  // check if green and black was detected
        deg += 90 * int(right && right_black);
        if (right && !(left)){
            deg *= -1;
        }

        // execute the turning      
        motor::fwd(120);
        if (shiftregister::get(SR_STBY1)){
            motor::gyro(deg);
        }
        motor::fwd(120);
        // set freeze
        last_green = millis();

        // reset LEDs
        rgb::setValue(Side::BOTH, 0, 0, 0);
        // reset color counters
        color::green.reset();
        color::black.reset();
    }
    // Red line - stop and wait 6s
    // if falsely detected, it will continue
    // with only losing some time
    if (color::red() != Side::NONE){
        motor::stop();
        rgb::setValue(Side::BOTH, 255, 0, 0);
        delay(6000);
        rgb::setValue(Side::BOTH, 0, 0, 0);
        color::red.reset();
    }
    
}
