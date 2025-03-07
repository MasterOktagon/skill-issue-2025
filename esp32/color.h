#pragma once

//
// COLOR HEADER
//
// This Header layouts the color detection interface
//

#include <vector>
#include <initializer_list>
#include <string>

#include "shared.h"
#include "lightsensor.h"

using namespace std;

#define CHECK_LEN 7             // check lenght required to detect a color
#define RED_THRESHOLD 30        // Thresholds to detect a color
#define GREEN_THRESHOLD 13      // ..
#define BLACK_THRESHOLD 37      // ..
#define SILVER_THRESHOLD 150    // ..
#define WHITE_THRESHOLD  70     // ..

namespace color{
/*
namespace that deals with color detection
*/
    class color{
    /*
    class that represents a color detection method
    */
        private:
            fsignal<bool, lightSensorArray*, lightSensorArray*, lightSensorArray*, Side> func;
            // function that is called to detect a color
            
            uint8_t counter_l, counter_r;
            // counters that are counted up when func returns true

        public:
            color(fsignal<bool, lightSensorArray*, lightSensorArray*, lightSensorArray*, Side> method);
            /*
            Constructor
            */
            
            void update(lightSensorArray* w, lightSensorArray* g, lightSensorArray* r);
            /*
            run an update on this color check
            */
            
            Side operator () ();
            Side get();
            /*
            return on which sides a color has been detected
            */

            void reset();
            /*
            reset counters
            */
    };

    extern color red, green, black, black_outer, silver, green_outer, white;
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

