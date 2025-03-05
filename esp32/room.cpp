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
    int8_t turn = v.angle;
    pinMode(MS, INPUT_PULLUP);
    
    motor::gyro(turn, V_STD);
    claw::wide(); claw::down(); claw::open();
    tof::enable(tof::tof::CLAW);
    motor::fwd(motor::motor::AB, V_STD);
    uint16_t tof_data;
    do {
        tof_data = tof::claw.readSingle();
    } while (tof_data > 69 || tof_data == 0); // 0 == timeout
    delay(100);
    motor::stop();
    claw::close();
    delay(1500);
    claw::up();

    if (analogRead(FLEX) >= 3170){
        output.println("INFO: Victim rescued");
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
    }

    return false;
}

void zone::loop(){
    motor::stop();
    output.println("INFO: Zone program started");

    uint8_t victim_counter = 0;
    
    rpi::start_ai(rpi::Ai::VICTIMS);
    while (victim_counter < 3) {
        if (rpi::status != 0x00) break;
        
        Victim v = rpi::get_victim();
        if (! (v.angle == 0 && v.dist == 0) ){
            victim_counter += uint8_t(takeVictim(v));
        }
    }
    rpi::stop_ai();
}


