#include "color.h"

//
// LINEFOLLOWER.cpp
//
// the heart of our program
//

#include <cstdint>
#include <list>
#include <numeric>
#include <sys/_stdint.h>

#include "lightsensor.h"
#include "linefollower.h"
#include "motor.h"
#include "gyro.h"
#include "fadc.h"
#include "tof.h"
#include "esp32-hal-gpio.h"
#include "shared.h"

using namespace std;

#define INNER_FACTOR 2
#define OUTER_FACTOR 4
#define BACK_FACTOR  1
#define GREEN_FACTOR 10

#define P            1.5
#define D            0.6
#define I            -0.01

#define MAX_I        10000

int16_t last = 0;
uint16_t tof_dist = 0;

int16_t max_abs(int16_t a, int16_t b){
    if (abs(a) > abs(b)) return a;
    return b;
}

int16_t lf::follow(){
    #ifdef LF_ACTIVE
        int16_t diff       = (ls::white.left.value - ls::white.right.value)             * INNER_FACTOR;
        int16_t diff_outer = (ls::white.left_outer.value - ls::white.right_outer.value) * OUTER_FACTOR;
        int16_t diff_back  = (ls::white_b.left.value - ls::white_b.right.value)         * BACK_FACTOR;
        int16_t diff_green = ((ls::green.left.value - ls::red.left.value) - (ls::green.right.value - ls::red.right.value)) * GREEN_FACTOR;

        int16_t mot_diff = max_abs(diff + diff_outer,diff_back) + diff_green; // TODO: maybe change diff_outer to negative factor

        int16_t d = mot_diff - last;
        last = mot_diff;

        static int64_t i = i + mot_diff;

        //if(color::black_outer() == Side::LEFT) mot_diff += 100;
        //else if(color::black_outer() == Side::RIGHT) mot_diff -= 100;

        //if (tof::front.dataReady()){
        //    tof_dist = tof::front.read(false);
        //    //output.print("TOF: "); output.println(tof_dist);
        //    tof::front.readSingle(false);
        //}

        int16_t v = V_STD;//tof_dist > 140 || tof_dist == 0 ? V_STD : 80;

        gyro::update();

        int16_t correction = (mot_diff * P - d * D - i * I)*0.5;
        if(abs(correction) <= 10) correction = 0;
        if(abs(correction) >= 170) v = -40;

        motor::fwd(motor::motor::A, v - correction);
        motor::fwd(motor::motor::B, v + correction);
    #endif

    return 0;
}
