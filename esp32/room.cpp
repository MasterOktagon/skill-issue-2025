
#include "color.h"
#include "room.h"
#include "motor.h"
#include "lightsensor.h"
#include "tof.h"

void zone::ignore(){
    motor::gyro(90, V_STD);
    motor::fwd(1000);
    while (true){
        ls::read();
        color::update();

        if(color::black() || color::black_outer()){
            motor::fwd(500);
            return;
        }

        if((!digitalRead(T_L) || !digitalRead(T_R)) && !button_failure){
            motor::rev(200);
            motor::gyro(-90, V_STD);
        }

        
    }
}



