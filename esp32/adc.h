#pragma once

//
// ADC MULTIPLEXER HEADER
//
// for CD74HC4067
//

#include <cstdint>
#include <Arduino.h>
#include <CD74HC4067.h>

#include "Pins.h"

namespace adc{
    /*
    namespace that interacts with the adc
    */

    extern void setup();
    /*
    set up necessary pins
    */

    extern int16_t read(uint8_t pin);
    /*
    read out the data of one pin
    */

}

