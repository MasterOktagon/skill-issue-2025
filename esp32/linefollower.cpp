
//
// LINEFOLLOWER.cpp
//
// the heart of our program
//

#include <cstdint>

#include "lightsensor.h"
#include "linefollower.h"
#include "motor.h"
#include "gyro.h"
#include "fadc.h"

using namespace std;

int16_t lf::follow(){
    static long timestamp = 0;
    #ifdef LF_ACTIVE
        #define diff_outer_factor 2  // Factor for the outer light
        #define mul 1 // general factor
        #ifdef USE_TIMESCALE
            float timescale;
            int16_t time_diff = micros() - timestamp;
            timestamp = micros();
            if (time_diff <= 500000) {  // time between loops, maximum 500ms to prevent "interesting" behaviour after e.g. obstacles
                timescale = float(time_diff) / 3000; // scale factor
            } else {
                timescale = 1;
            }

        #else
            #define timescale 1
        #endif

        static int16_t last;     // last computed difference
        static int16_t bias = 0; // bias (Integral)
        int16_t mot_diff;        // output diff

        // PID line follower
        #ifdef LF_USE_BACK
            int16_t diff_back = ls::white_b.left.value - ls::white_b.right.value;
        #endif
        int16_t diff = ls::white.left.value - ls::white.right.value;
        int16_t diff_green = (ls::green.left.value - ls::red.left.value) - (ls::green.right.value - ls::red.right.value);  // difference to ignore green points
        int16_t diff_outer = ls::white.left_outer.value - ls::white.right_outer.value;
        mot_diff = int(float((diff + diff_green * 2) * 4 + diff_outer * diff_outer_factor) * mul * timescale);  // calculate inner to outer mult
        int16_t derivative = int(float(mot_diff - last / timescale));
        bias += mot_diff;
        //#define bias 0

        #ifndef MOT_STBY
            //int16_t v = V_STD;
            //if(abs(mot_diff) < 5){
            //    v = 250;
            //}
            int16_t v = V_STD - abs(mot_diff);

            #ifdef LF_USE_BACK
                float scale = 1 / ((abs(mot_diff) + 0.1) * 0.25);  // only use back LS if no difference on front is applied

                int16_t vback_a = int(diff_back) * scale;
                int16_t vback_b = int(diff_back) * scale;
            #else
                #define vback_a 0
                #define vback_b 0
            #endif

            // start motors
            if (abs(gyro::x) > 15) { // reduce correction on ramps
                motor::fwd(motor::motor::A, v + (mot_diff - derivative * 2 + bias * 0.002 + vback_a) * 0.3);
                motor::fwd(motor::motor::B, v - (mot_diff + derivative * 2 - bias * 0.002 - vback_b) * 0.3);

            } else {
                motor::fwd(motor::motor::A, v + mot_diff - derivative * 2 + bias * 0.002 + vback_a);
                motor::fwd(motor::motor::B, v - mot_diff + derivative * 2 - bias * 0.002 - vback_b);
            }
            last = mot_diff;
        #endif

        return 0;//diff_green;// - derivative * 2 + bias * 0.01 + vback_a;
    #else
        return 0;
    #endif
}
