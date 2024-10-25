#pragma once

//
// RPI_COM.h
//
// This header holds the communication protocol with the raspberry pi
//

#include <Arduino.h>
#include <Wire.h>
#include <vector> 

using namespace std;

struct Victim {
/*
Victim positionon image
*/
    uint16_t x,y,r = 0;
}

namespace rpi {
/*
namespace that holds raspberry pi comm fucntions
*/

    extern void setup();
    /*
    start communication
    */

    enum Ai {
    /*
    select which ai to start
    */
        NONE    = 0x0,
        VICTIMS = 0x1,
        CORNERS = 0x2,
        EXITS   = 0x3
    };

    extern void start_ai(Ai ai);
    /*
    start the ai

    [param ai] ai to start
    */

    extern void stop_ai();
    /*
    stop any ai
    */

    extern Victim get_victim();
    /*
    get the data from the victim ai

    [return] nearest victim information
    */

}



