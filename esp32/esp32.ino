
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
#include "room.h"

using namespace std;

#define GREEN_TIMEOUT 100

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
        delay(10);
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
    output.println("INFO: output init [115200] ...");

    output.println("INFO: Resetting LEDs ...");
    digitalWrite(PT_RED,       LOW);
    digitalWrite(PT_GREEN,     LOW);
    digitalWrite(PT_WHITE_L,   LOW);
    digitalWrite(PT_WHITE_R,   LOW);
    digitalWrite(PT_WHITE_REF, LOW);
    
    // init SR
    output.println("INFO: Shiftregister init...");
    shiftregister::setup();
    shiftregister::reset();
    motor::stop();

    output.println("INFO: Wire init...");
    Wire.begin(SDA, SCL);  // start i2c
    Wire.setClock(400000); // Fast mode

    // setup FastADC
    #ifdef FASTREAD
        output.println("INFO: FADC begin...");
        fadc::begin();
    #endif
    
    // create lightSensor objects
    ls::setup();

    output.println("INFO: Display init...");
    if (!menu::DisplayInit()){
        rgb::setValue(Side::BOTH, 255, 0, 0);
        output.println("ERROR: Display init failed!");
    }

    tof::setup();

    // run the led test
    #ifdef LED_TEST
        output.println("INFO: LED test (wwgr)");
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

    output.println("INFO: Initializing gyro...");
    gyro::init();

    output.println("INFO: PWM bus init");
    claw::setup();
    rgb::setValue(Side::BOTH, 0, 0, 0);

    #ifdef CLAW_TEST
        output.println("INFO: Claw Test...");
        output.println("\tOpen");
        claw::open();
        output.println("\tClose");
        claw::close();
        output.println("\tWide");
        claw::wide();
        output.println("\tClose");
        claw::close();

        claw::up();
        claw::down();
    #endif
    #ifdef STORAGE_TEST
        storage::unload(Side::LEFT);
        storage::divide(Side::LEFT);
        //delay(1500);
        storage::unload(Side::RIGHT);
        storage::divide(Side::RIGHT);
        //delay(1500);
    #endif
    storage::reset();

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

    output.println("INFO: Checking Buttons for failures...");
    // if a button has failed (is pressed when it shouldn't be)
    // we disable buttons by setting the button_failure flag
    pinMode(T_L, INPUT_PULLUP);
    pinMode(T_R, INPUT_PULLUP);
    if (!digitalRead(T_L) || !digitalRead(T_R)){
        output.println("ERROR: Button failure detected, disabling buttons!");
        menu::showWaiting("Button failure detected, disabling buttons!");
        rgb::setValue(Side::BOTH, 255, 0, 0);
        delay(1000);
        button_failure = true;
    }
    output.print("PI: Status: ");
    output.println(rpi::status());

    
    // menu selection
    output.println("INFO: Menu");

    digitalWrite(PT_GREEN, HIGH);
    int selected = 0;
    rgb::setValue(Side::BOTH, 0,0,0);
    #ifdef MENU
        while((selected = menu::menu(button_failure)) != MENU_RUN){
            Serial.println("sdfdfsfdfs");
            switch (selected){
                default:
                    break;

                case 3:
                    delay(1500);
                    attachInterrupt(T_E, isr, RISING);
                    zone::takeVictim(0);//ignore();
                    detachInterrupt(T_E);
                    break;

                case MENU_CALIBRATE:

                    menu::showWaiting("Calibrating...");
                    delay(1500);

                    attachInterrupt(T_E, isr, RISING);
                    thread t(cal_movement);
                        ls::calibrate(5000, 0);
                    t.join();
                    //cal_movement();
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
    #else
        delay(1500);
    #endif

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
    shiftregister::set(SR_XSHT_2, HIGH);
    delay(2000);
    attachInterrupt(T_E, isr, RISING);
    tof::front.readSingle(false);
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

unsigned long green_freeze = 0;

void loop(){
    

    thread t(lf::follow);
        ls::read(false);
        color::update();
    t.join();
    ls::update();

    if (color::red()){
        output.println("LFE: RED detected");
        motor::stop();
        menu::showWaiting("RED");
        rgb::setValue(Side::BOTH, 255, 0, 0);
        delay(6000);
        rgb::reset();
        color::red.reset();
    }

    if (color::green() && green_freeze < millis()){
        Side green = Side(color::green());
        menu::showWaiting(match(green));
        motor::fwd(motor::motor::AB, 70);
        do {
            ls::read();
            color::update();
            green = Side(green | color::green());
        } while(color::green());
        motor::read_fwd(70, 5, {&color::black, &color::black_outer});
        Side black = Side(color::black() | color::black_outer());
        motor::stop();

        Side turn = Side(green & black);
        rgb::setValue(turn, 0, 255, 0);
        output.print("LFE: GREEN detected "); output.println(match(turn));
        menu::showWaiting(match(turn));

        int16_t deg = 85 * bool(turn & Side::LEFT);
        deg += 85 * bool(turn & Side::RIGHT) * (turn & Side::LEFT ? 1 : -1);

        if(deg != 0){
            motor::fwd(200);
            motor::gyro(deg);
            //motor::fwd(20);
            green_freeze = millis() + 250;
        }
        color::green.reset();
        rgb::reset();
    }

    //output.println(ls::red.right.value - ls::green.right.value);//float((ls::green.left.raw - ls::red.left.raw)) / (ls::rg_min_l - ls::rg_max_l) * -100);
    //output.print("\t");
    //output.println(ls::green.right.value);
    //output.println(mpu.getGyroY());
    //output.println();
    if ((!digitalRead(T_L) || !digitalRead(T_R)) && !button_failure){ // check for obstacle using the buttons // TODO: slow down using the TOF-sensors
        motor::rev(100);
        output.println("LFE: OBSTACLE detected");
        motor::gyro(-80);
        unsigned long timestamp = millis() + 2700;
        motor::fwd(motor::motor::A, 250);
        motor::fwd(motor::motor::B, 57);
        while(timestamp > millis()){
                ls::read();
                color::update();
                if((color::black() || color::black_outer()) && (timestamp-4000) < millis()) break;
        }
        delay(300);
        motor::gyro(-45);
    }
}
