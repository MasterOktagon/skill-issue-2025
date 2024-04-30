#pragma once
#include <cstdint>
#include <Arduino.h>

#include "Pins.h"

namespace shiftregister{
    /*
    Namespace to interact with the shiftregisters
    */
    
    extern bool setup();
    /*
    Setup the shiftregister
    
    !Warns! shiftregisterWarning
    
    [return] true if succes
    */
    
    extern void set(const uint8_t pin, bool state, bool flush=true);
    /*
    Set the State of a shiftregister pin
    
    [param pin] pin to set
    [param state] state to set the pin to
    [param flush] wether to update the shiftregister afterwards
    */
    
    extern bool get(const uint8_t pin);
    /*
    Get the state of a pin
    */
    
    extern void reset();
    /*
    Reset all pins to LOW
    */
    
    extern bool is_flushed();
    /*
    Wether the Hardware is up to date with the bits
    */

}




