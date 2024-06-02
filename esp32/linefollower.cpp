
#include <queue>
#include <cstdint>

#include "lightsensor.h"
#include "linefollower.h"
#include "motor.h"

using namespace std;

#define LF_ACTIVE

void lf::follow(){
    
    #ifdef LF_ACTIVE
    #define diff_outer_factor 2  // Factor for the outer light
    #define mul 1
    #ifdef USE_TIMESCALE
        double timescale;
        if (micros() - timestamp <= 500000) {  // maximum 500ms
            timescale = float(micros() - timestamp) / 3000;
        } else {
            timescale = 1;
        }

    #else
    #define timescale 1
    #endif
    //static queue<int16_t> diffs{};
    
    //diffs.push();

    static int16_t last;
    int16_t mot_diff;
    //Serial.println("Gegremgremg");
    #ifdef LF_USE_BACK
        int16_t diff_back = ls::back.left.value - ls::back.right.value;
    #endif
    int16_t diff = (ls::white.left.value - ls::white.center.value) - (ls::white.right.value - ls::white.center.value);
    int16_t diff_green = (ls::green.left.value - ls::red.left.value) - (ls::green.right.value - ls::red.right.value);  // difference to ignore green value
    int16_t diff_outer = ls::white.left_outer.value - ls::white.right_outer.value;
    mot_diff = int(float((diff + diff_green * 2) * 4 + diff_outer * diff_outer_factor) * mul * timescale);  // calculate inner to outer mult
    int16_t derivative = int(float(mot_diff - last / timescale));
    //diff_interchange = mot_diff;
    #ifndef NOMOTORS
        int16_t v = min(max(V_STD, int(V_STD - mot_diff * 0.5)), 50);  // scale base speed based on difference and gyro speed

        #ifdef LF_USE_BACK
            float scale = 1 / ((abs(mot_diff) + 0.1) * 0.25);  // only use back LS if no difference

            int16_t vback_a = int(diff_back) * scale;  //min(0, int(diff_back)) * scale;
            int16_t vback_b = int(diff_back) * scale;  //max(0, int(diff_back)) * scale;
        #else
        #define vback_a 0
        #define vback_b 0
    #endif

    // start motors

    /*if (abs(gyro::XAngle) > 15) {
      motor::fwd(A, v + gyro::XAngle + mot_diff * 0.3 + vback_a);
      motor::fwd(B, v + gyro::XAngle - mot_diff * 0.3 - vback_b);
    }*/
    if(false){
    } else {
        //Serial.println("Gegremgremg");
        motor::fwd(motor::motor::A, v + mot_diff - derivative * 2 + vback_a);
        motor::fwd(motor::motor::B, v - mot_diff + derivative * 2 - vback_b);
    }



    last = mot_diff;
    #endif
    //    diff_interchange = mot_diff;
    #endif

}
