#pragma once

//
// FADC HEADER
//
// custom reimplentation of the fastAnalogRead from stg
// https://github.com/stg/ESP32-S3-FastAnalogRead
// 
// NOTES:
// not C-compatible!!
// maximum Board version: 2.0.17
//

#include <cstdint>

#define FADC_RESOLUTION 12 // 12-bit resolution
#define FADC_ATTEN ADC_11db
#ifdef FASTREAD
    #define analogRead(pin) fadc::read(pin) // replace the standard analogRead with the Fast read
#endif

namespace fadc {
    extern void begin();
    /*
    initialize the fadc module
    */

    extern uint16_t read(uint8_t pin);
    /*
    start an async read and wait until it is completed

    [param pin] pin to read out. needs an adc channel (gpio 1-20)
    */

    extern void start(uint8_t pin);
    /*
    start an async read

    [param pin] pin to read out. needs an adc channel (gpio 1-20)
    */

    extern bool busy();
    /*
    check if the async read is completed
    */

    extern uint16_t value();
    /*
    read out the last read value
    */
}





