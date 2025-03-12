#pragma once
#include "rpi_com.h"

//
// ROOM.h
//
// layouts the rescue room program
//

namespace zone {
    extern void ignore();
    /*
    skip the room
    */
    extern bool takeVictim(Victim v);
    /*
    take a victim
    */
    extern void loop();
    /*
    zone main loop
    */
    extern void unload(fsignal<int8_t> fn);
}




