#pragma once

//
// LINEFOLLOWER HEADER
//
// we had to move our linefollower into a seperate file
//

#include <Arduino.h>
#include <cstdint>

#include "shared.h"
#include "shiftregister.h"

namespace lf{
/*
namespace that holds the linefollower
*/

    extern int16_t follow();
    /*
    follow the line
    */
}

