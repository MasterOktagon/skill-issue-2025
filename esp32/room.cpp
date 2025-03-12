#include "rpi_com.h"
#include "esp32-hal-adc.h"
#include "shared.h"
#include "esp32-hal-gpio.h"
#include "Pins.h"
#include "shiftregister.h"

#include "color.h"
#include "room.h"
#include "motor.h"
#include "lightsensor.h"
#include "tof.h"
#include "shiftregister.h"
#include "claw.h"
#include "menu.h"
#include <thread>

void zone::ignore(){
    int16_t tof_dist = 100;
    int16_t tof_last = 100;
    shiftregister::set(SR_XSHT_1, HIGH, false);
    shiftregister::set(SR_XSHT_4, LOW);

    motor::gyro(90, V_STD);
    motor::fwd(1000);
    tof::left.readSingle(false);
    while (true){
        motor::fwd(motor::motor::A, V_STD + ((tof_dist != 0? tof_dist : 90)-90));
        motor::fwd(motor::motor::B, V_STD - ((tof_dist != 0? tof_dist : 90)-90));
        ls::read();
        color::update();

        if (tof::left.dataReady()){
            tof_last = tof_dist;
            tof_dist = tof::left.read(false);
            output.print("TOF: "); output.println(tof_dist);
            tof::left.readSingle(false);
        }

        if(color::black() | color::black_outer()){
            //motor::fwd(50);
            //while(!color::black() & Side::RIGHT){
            //    ls::read();
            //    color::update();
            //}
            if(!color::black() == Side::BOTH){
                motor::gyro(30, V_STD);
            } 
            return;
        }

        if((!digitalRead(T_L)) && !button_failure){
            //motor::stop();
            motor::rev(100);
            while (!digitalRead(T_L) || digitalRead(T_R)){
                motor::rev(150);
                motor::fwd(motor::motor::A, V_STD);
                motor::fwd(motor::motor::B, 60);
                while (digitalRead(T_L) && digitalRead(T_R));
                motor::fwd(50);
            }
            //motor::gyro(15);
            motor::fwd(80);
            motor::rev(50);
            motor::gyro(-90, V_STD);
        }

        if(tof_dist > 200 || tof_dist == 0 && tof_last == 0){
            motor::fwd(550);
            motor::gyro(85, V_STD);
            tof_dist = 100;
            motor::fwd(motor::motor::AB, V_STD);
            while (!(!digitalRead(T_L) || !digitalRead(T_R)) && !button_failure) {
                ls::read();
                color::update();

                if(color::black() | color::black_outer()){
                    motor::fwd(250);
                    motor::stop();
                    return;
                }
            }

            motor::rev(100);
            motor::gyro(-85, V_STD);
            tof_dist = 100;
        
        }

        
    }
}

bool zone::takeVictim(Victim v){
    output.println("ball aufnehmen");
    int8_t turn = v.angle;
    pinMode(MS, INPUT_PULLUP);

    if(v.dist < 200){
        motor::fwd(motor::motor::A, V_STD + v.angle);
        motor::fwd(motor::motor::B, V_STD - v.angle);
        timer t(150);
        while (!t.expired()){
            ls::read();
            color::update();

            if (color::black() || color::silver() /*|| !digitalRead(T_L) || !digitalRead(T_R)*/) {
                motor::rev(5cm);
                motor::gyro(180);
                color::silver.reset(); color::black.reset();
            }
        }
        
        motor::stop();
        return false;
    }
    if (abs(turn) > 5) motor::gyro(turn, V_STD);
    motor::rev(200);
    claw::wide(); claw::down(); delay(200); claw::open();
    int16_t a = analogRead(FLEX);
    tof::enable(tof::tof::CLAW);
    motor::fwd(motor::motor::AB, V_STD);
    uint16_t tof_data;
    timer timeout(1200);
    do {
        tof_data = tof::claw.readSingle();
        if (timeout.expired()){
            motor::stop();
            claw::up();
            claw::close();
            return false;
        }
    } while (tof_data > 69 || tof_data == 0); // 0 == timeout
    delay(200);
    motor::stop();
    claw::close();
    delay(1500);
    claw::up();

    if (abs(analogRead(FLEX) - a) > 70){
        output.println("INFO: Victim rescued");
        menu::showWaiting("Victim rescued");
        if (digitalRead(MS) + digitalRead(MS) == 0){
            storage::divide(LEFT);
        }
        else {
            storage::divide(RIGHT);
        }

        claw::wide();
        delay(1000);
        claw::close();
        return true;
    }
    else { 
        output.println("WARNING: Victim not rescued");
        menu::showWaiting("Victim not rescued");
    }

    return false;
}

void thread_fn(Victim v , uint8_t* counter){
    if (! (v.angle == 0 && v.dist == 0) ){
        *counter += uint8_t(zone::takeVictim(v));
    }
}

void zone::unload(fsignal<int8_t> detect_fn){
    rpi::start_ai(rpi::Ai::CORNERS);
    motor::fwd(motor::motor::AB,100);
    while (true){
        int8_t corners = detect_fn();
        if (corners != 0x7E){
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
                    
}

void zone::loop(){
    motor::stop();
    rgb::highbeam(HIGH);
    output.println("INFO: Zone program started");

    uint8_t victim_counter = 0;
    
    rpi::start_ai(rpi::Ai::VICTIMS);
    Victim last, v;
    while (victim_counter < 3) {
        output.println("uwu");

        v = rpi::get_victim();
        timer frametime(500);
        thread_fn(v, &victim_counter);
        while (!frametime.expired());

        /*if (last.angle == 0 && last.dist == 0 && v.angle == 0 && v.dist == 0){
            timer t(600);
            motor::fwd(motor::motor::AB, V_STD);
            motor::gyro(45);
            while (!t.expired()){
                ls::read();
                color::update();

                if (color::black() || color::silver() || !digitalRead(T_L) || !digitalRead(T_R)) {
                    motor::rev(5cm);
                    motor::gyro(180);
                    color::silver.reset(); color::black.reset();
                }
            }
        }*/

        last = v;
        zone::unload(rpi::get_corner_green);
        zone::unload(rpi::get_corner_red);
        motor::fwd(5cm);
        motor::gyro(90);
        zone::ignore();
    }



    rpi::stop_ai();
}


