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

namespace rgb {
/*
namespace that controls the onboard debug RGB LEDs [W.I.P.]
*/
}

