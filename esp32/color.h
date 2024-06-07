#pragma once

#include <vector>
#include <initializer_list>
#include <string>

#include "shared.h"
#include "lightsensor.h"

using namespace std;

#define CHECK_LEN 7
#define RED_THRESHOLD 30
#define GREEN_THRESHOLD 25
#define BLACK_THRESHOLD 15

namespace color{
/*
namespace that deals with color detection
*/
    class color{
    /*
    class that represents a color detection method
    */
        fsignal<bool, lightSensorArray*, lightSensorArray*, lightSensorArray*, Side> func;
        uint8_t counter_l, counter_r;

        public:
        color(fsignal<bool, lightSensorArray*, lightSensorArray*, lightSensorArray*, Side> method);

        void update(lightSensorArray* w, lightSensorArray* g, lightSensorArray* r);
        /*
        run an update on this color check
        */
        
        Side operator () ();
    };

    extern color red, green, black;
    extern vector<color&> colors; // all colors will be registered here
    
    void update();
    /*
    update all colors to be checked
    */
    void update(initializer_list<color*> colors);
    /*
    update all given colors to be checked
    */
}

