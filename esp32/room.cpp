#include "esp32-hal-gpio.h"
#include "Pins.h"
#include "shiftregister.h"

#include "color.h"
#include "room.h"
#include "motor.h"
#include "lightsensor.h"
#include "tof.h"
#include "shiftregister.h"

void zone::ignore(){
    int16_t tof_dist = 100;
    int16_t tof_last = 100;
    shiftregister::set(SR_XSHT_1, HIGH, false);
    shiftregister::set(SR_XSHT_4, LOW);

    motor::gyro(90, V_STD);
    motor::fwd(1000);
    tof::left.readSingle(false);
    while (true){
        motor::fwd(motor::motor::AB, V_STD);
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



