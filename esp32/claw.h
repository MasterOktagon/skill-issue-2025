
#pragma once

#include <Bonezegei_PCA9685.h>
#include <Arduino.h>
#include <cstdint>



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

