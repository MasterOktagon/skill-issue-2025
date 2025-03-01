#pragma once

//
// TOF.h
//
// implements a high-level acces to the time-of-flight sensors
// This is the special implementaition for the VL53L1X ToF sensor
//

#include <Arduino.h>
#include <VL53L1X.h>
#include "Pins.h"
#include "shiftregister.h"

namespace tof {
/*
implements a high-level acces to the time-of-flight sensors
*/
    extern VL53L1X front, left, right, claw;

    void setup();
    /*
    set up light sensors
    */

    void start_all();
    /*
    start continuous reading on all ToFs
    */
    
    void stop_all();
    /*
    stop continuous reading on all ToFs
    */

    enum tof {
        OFF,
        FRONT,
        LEFT,
        RIGHT,
        CLAW
    };

    void enable(tof);
    /*
    enable a certain TOF
    */
}




