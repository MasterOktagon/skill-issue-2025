
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
timer green_freeze;
timer white_timer;

long timestamp;

void IRAM_ATTR isr(){
    static uint8_t counter = 0;
    counter++;
    if (counter > 1) esp_restart();
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
    Wire.setClock(100000); // Fast mode

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
    rgb::highbeam(LOW);

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
    Wire.setTimeout(100000);
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
                    //rgb::highbeam(HIGH);
                    //zone::loop();
                    rpi::start_ai(rpi::Ai::CORNERS);
                    motor::fwd(motor::motor::AB,100);
                    while (true){
                        int8_t corners = rpi::get_corner_red();
                        if (corners != 0xFF){
                            output.println(corners);
                            if (abs(corners) > 10) {
                                motor::fwd(motor::motor::A, V_STD + corners*2 );
                                motor::fwd(motor::motor::B, V_STD - corners*2 );
                            }

                            if(!digitalRead(T_L) || !digitalRead(T_R)) break;
                        }
                        else {motor::stop();}
                        delay(10);
                    }
                    output.println("INFO: Corner detected");
                    motor::stop();
                    while(digitalRead(T_L) || digitalRead(T_R)){
                        while(digitalRead(T_R)){
                            motor::fwd(motor::motor::A, V_STD);
                            motor::rev(motor::motor::B, 70);
                        }
                        while(digitalRead(T_L)){
                            motor::fwd(motor::motor::B, V_STD);
                            motor::rev(motor::motor::A, 70);
                        }
                        delay(10);
                    }
                    motor::fwd(motor::motor::A, V_STD);
                    delay(500);
                    motor::fwd(motor::motor::B, V_STD);
                    delay(500);
                    motor::stop();

                    motor::rev(7cm);
                    motor::gyro(180);
                    motor::rev(7cm);
                    storage::unload(Side::LEFT);
                    storage::unload(Side::NONE);

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
    green_freeze.reset();
    white_timer.set(25cm);
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

//unsigned long green_freeze = 0;

void loop(){
    static int16_t last_gap_correction;

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

    if (color::silver()){
        motor::stop();
        menu::showWaiting("SILVER");
        output.println("LFE: Silver detected");
        delay(1000);
    }

    if (color::white() != Side::BOTH){
        white_timer.reset();
        last_gap_correction = 5;
    }
    else if (white_timer.expired()){
        motor::stop();
        menu::showWaiting("GAP");
        output.println("LFE: Gap detected");
        motor::rev(25cm);
        motor::gyro(last_gap_correction);
        last_gap_correction *= -1.5;
        white_timer.reset();
    }

    if (color::green() && /*green_freeze < millis()*/ green_freeze.expired() ){
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
            //green_freeze = millis() + 250;
            green_freeze.set(250);
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
        motor::fwd(motor::motor::A, 250);
        motor::fwd(motor::motor::B, 45);
        timer turn_timer(6000);
        while( !turn_timer.expired() ){
                ls::read();
                color::update();
                if((color::black() || color::black_outer()) && turn_timer.passed() > 2000) break;
        }
        delay(500);
        motor::gyro(-60);
    }
}
