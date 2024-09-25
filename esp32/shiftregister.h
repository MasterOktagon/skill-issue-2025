#pragma once

//
// SHIFTREGISTER HEADER
//
// layouts the shiftregister interface. Its battle-proved.
// can be used outside of this project (remember the license) with minor modifications:
//      - comment out or remove '#include "shared.h"', this only replaces the Serial output with
//           an OutputWriter
//      - provide your own Pins.h which defines STCP, SHCP, DS and SR_PORTS macros
//      - dont forget including the shiftregister.cpp file in your directory
//

#include <cstdint>
#include <Arduino.h>

#include "Pins.h"
#include "shared.h"

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
    
    extern void shift();
    /*
    update the register.
    */

    extern bool is_flushed();
    /*
    Wether the Hardware is up to date with the bits
    */

}




