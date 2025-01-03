
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
#define OUTER_FACTOR 2
#define D            0.3
#define I            -0.0003

int16_t last = 0;
uint16_t tof_dist;

int16_t lf::follow(){
    #ifdef LF_ACTIVE
        int16_t diff       = (ls::white.left.value - ls::white.right.value)             * INNER_FACTOR;
        int16_t diff_outer = (ls::white.left_outer.value - ls::white.right_outer.value) * OUTER_FACTOR;

        int16_t mot_diff = diff + diff_outer; // TODO: maybe change diff_outer to negative factor

        int16_t d = mot_diff - last;
        last = mot_diff;

        static int64_t i = i + mot_diff;

        if (tof::front.dataReady()){
            tof_dist = tof::front.read(false);
            output.print("TOF: "); output.println(tof_dist);
            tof::front.readSingle(false);
        }

        int16_t v = tof_dist > 140 || tof_dist == 0 ? V_STD : 80;

        gyro::update();

        motor::fwd(motor::motor::A, V_STD - mot_diff - d * D - i * I);
        motor::fwd(motor::motor::B, V_STD + mot_diff + d * D + i * I);
    #endif

    return 0;
}
