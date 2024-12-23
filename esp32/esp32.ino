
// 
// MAIN PROGRAM
// licensed under CC-BY-NC 4.0
// 
// copyright (c) SkillIssue Team, Berlin, 2024
//
// TODO: clibrate with difference, check with white

#include <Wire.h>
#include <thread>
#include <exception>
#include <string>

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
    while(millis() - timestamp < 20000){
        int v = ((millis() - timestamp) % 4000) >= 2000 ? -70 : 70;
        //output.println(v);
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

        claw::up();
        claw::down();
    #endif
    #ifdef STORAGE_TEST
        storage::unload(Side::LEFT);
    #endif
    #ifndef NO_CLAW
        claw::up();
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

    //digitalWrite(PT_GREEN, HIGH);
    int selected = 0;
    rgb::setValue(Side::BOTH, 0,0,0);
    while((selected = menu::menu(button_failure)) != MENU_RUN){
        switch (selected){
            default:
                break;

            case MENU_CALIBRATE:

                menu::showWaiting("Calibrating...");
                delay(1500);

                attachInterrupt(T_E, isr, RISING);
                thread t(cal_movement);
                    ls::calibrate(7000, 0);
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

const char* match(Side s){
    switch (s){
        case Side::LEFT:
            return "LEFT";
        case Side::RIGHT:
            return "RIGHT";
        case Side::BOTH:
            return "BOTH";
        default:
            return "NONE";
    }
    return "";
}

void loop(){
    

    thread t(lf::follow);
        ls::read(false);
        color::update();
    t.join();
    ls::update();
    gyro::update();

    if (color::silver()){
        output.println("SILVER");
        motor::fwd(500);
        delay(5000);
    }

    if (color::red()){
        motor::stop();
        rgb::setValue(Side::BOTH, 255, 0, 0);
        delay(6000);
        rgb::reset();
        color::red.reset();
    }

    if (color::green()){
        Side green = color::green();
        motor::fwd(motor::motor::AB, 70);
        do {
            ls::read();
            color::update();
            green = Side(green | color::green());
        } while(color::green());
        delay(100);
        Side black = Side(color::black() | color::black_outer());
        motor::stop();

        Side turn = Side(green & black);
        output.print("GREEN "); output.println(match(turn));
        menu::showWaiting(match(turn));

        int16_t deg = 90 * bool(turn & Side::LEFT);
        deg += 90 * bool(turn & Side::RIGHT) * (turn & Side::LEFT ? 1 : -1);

        if(deg != 0){
            motor::fwd(170);
            motor::gyro(deg);
            motor::fwd(50);
        }
        color::green.reset();
    }

    if (!(digitalRead(T_L) && digitalRead(T_R)) && !button_failure){ // check for obstacle using the buttons // TODO: slow down using the TOF-sensors
        motor::rev(100);
        motor::gyro(-60);
        motor::fwd(120);
        motor::fwd(motor::motor::A, 20);
        motor::fwd(motor::motor::A, 140);
        while(!color::black()){
            ls::read();
            color::update();
        }
        motor::fwd(50);
    }
}
