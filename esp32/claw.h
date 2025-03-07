#pragma once

//
// CLAW HEADER
//
// Header that layouts all functions related to the PWM Bus (claw and onboard LEDs)
// 

#include <Bonezegei_PCA9685.h>
#include <Arduino.h>
#include <cstdint>
#include "shared.h"


namespace claw {
/*
namespace that handles claw movement
*/
    
    extern void setup();

    extern void up();
    extern void down();
    extern void half();
    extern void open();
    extern void close();
    extern void wide();

}

namespace storage {
/*
namespace that holds storage functions
*/
    extern void divide(Side s, bool wide=false);
    /*
    turn the divider.
    
    [param s] Side to turn to. Side::LEFT clears the path to the left -> turns right
    [param wide] turn wide (to push victims)
    */

    extern void unload(Side s);
    /*
    unload side
    
    [param s] Side to unload. consistent with divide
    */

    extern void reset();
    /*
    close unloading mechanism. same as unload(Side::NONE)
    */
}

namespace rgb {
/*
namespace that controls the onboard debug RGB LEDs [W.I.P.]
*/
    extern void setValue(Side s,uint8_t r, uint8_t g, uint8_t b);
    extern void reset(Side s=Side::BOTH);
    extern void highbeam(bool status);
}

