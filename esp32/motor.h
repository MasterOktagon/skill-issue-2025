#pragma once

//
// MOTOR HEADER
//
// layouts the interface with the motors
//

#include <Arduino.h>
#include <cstdint>

#include "warnings.h"
#include "shiftregister.h"
#include "gyro.h"
#include "color.h"

// undef those because those literals are used here (prob. not a good idea)
#undef A1
#undef B1

namespace motor{
/*
This namespace holds all functions that directly influence motor movement
*/
    
    #define V_STD 200 // standard speed
    
    enum motor{
        /*
        This enum holds all possible motors as a possible value. A is left and 
        B is right. 1 is front and 2 is back.
        */    
        A1 = 1,
        A2 = 2,
        A  = 3,  // A = A1 | A2
        B1 = 4,
        B2 = 8,
        B  = 12, // B = B1 | B2
        AB = 15  // AB = A | B
    };
    
    
    extern void stop(motor m, bool hard = false);
    /*
    Stop a motor

    [param hard] short break if true
    */
    
    extern void stop(bool hard = false);
    /*
    stop all motors

    [param hard] if to short brake
    */

    extern void hardstop();
    /*
    stop all motors hard
    */
    
    extern void fwd(motor m, int16_t v);
    /*
    turn on motors in forward direction
    
    [param v] speed of the motors, negative results in reversed movement. clamped to -255..255.
    */
    
    extern void fwd(uint32_t time);
    /*
    go straight forward for the time given using V_STD
    
    [param time] time in ms.
    */
    
    extern void rev(motor m, int16_t v);
    /*
    turn on motors in reversed direction
    
    [param v] speed of the motors, negative results in forward movement. clamped to -255..255.
    */
    
    extern void rev(uint32_t time);
    /*
    go straight reversed for the time given using V_STD
    
    [param time] time in ms.
    */
    
    extern void turn(int16_t v);
    /*
    turn on axis
    
    [param v] speed of the motors, negative results in clockwise movement. clamped to -255..255. positive results in left turns
    */
    
    extern void gyro(int16_t angle, uint16_t v=V_STD, bool reset_gyro=true);
    /*
    turn on axis using the gyro
    
    [param angle] turn angle in degrees. Positive values result in left turns
    [param v] turn speed. only unsigned values are accepted because it would break otherwise. clamped between 0..255
    [param reset_gyro] reset the gyro afterwards
    */
    
    extern void standby(bool active);
    /*
    Turn the motors on standby/deactivate standby
    
    !Warns! standbyWarning
    */
    
    extern bool sensor_fwd(int16_t v, uint32_t time, initializer_list<color::color*> colors, Side side = Side::BOTH);
    /*
    Go straight forward reading and abort if timeout or color has been detected
    
    [param v] speed. negative will result in reversed movement
    [param time] time in ms before timeout
    [param colors] colors to read and check for
    
    [return] true if a color has been detected
    */

    extern void read_fwd(int16_t v, uint32_t time, initializer_list<color::color*> colors);
    /*
    go fwd while reading out colors

    [param v] speed. negative will result in reversed movement
    [param time] time in ms before timeout
    [param colors] colors to read and check for
    */

}


