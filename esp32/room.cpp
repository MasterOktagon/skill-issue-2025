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
            tof_dist = tof::left.read(false);
            //output.print("TOF: "); output.println(tof_dist);
            tof::left.readSingle(false);
        }

        if(color::black() || color::black_outer()){
            motor::fwd(100);
            return;
        }

        if((!digitalRead(T_L)) && !button_failure){
            while (digitalRead(T_R)){
                motor::fwd(motor::motor::A, 70);
                delay(50);
                motor::rev(50);
            }
            motor::fwd(80);
            motor::rev(250);
            motor::gyro(-90, V_STD);
        }

        if(tof_dist > 300 || tof_dist == 0){
            motor::fwd(400);
            motor::gyro(90, V_STD);
            tof_dist = 100;
            motor::fwd(motor::motor::AB, V_STD);
            while (!(!digitalRead(T_L) || !digitalRead(T_R)) && !button_failure) {
                ls::read();
                color::update();

                if(color::black() || color::black_outer()){
                    motor::fwd(100);
                    motor::stop();
                    return;
                }
            }

            motor::rev(250);
            motor::gyro(-90, V_STD);
        
        }

        
    }
}



