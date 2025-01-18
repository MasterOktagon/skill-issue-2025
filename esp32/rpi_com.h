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
    int8_t angle = 0;
    uint8_t dist = 0;
};

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
        NONE    = 0x00,
        VICTIMS = 0x01,
        CORNERS = 0x02,
        EXITS   = 0x03
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

    !raises! AINotStartedException
    [return] nearest victim information
    */

    extern uint8_t status();
    /*
    get the pi's status message

    0xFF - everything fine
    */

    class AINotStartedException : exception{
        public:
            Ai ai;
            AINotStartedException(Ai ai);
            const char * what();
    };

}



