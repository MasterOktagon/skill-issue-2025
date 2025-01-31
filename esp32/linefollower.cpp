
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

#define INNER_FACTOR 1
#define OUTER_FACTOR 5
#define BACK_FACTOR  1
#define GREEN_FACTOR -1

#define P            1
#define D            1
#define I            -0.0001

int16_t last = 0;
uint16_t tof_dist = 0;

int16_t max_abs(int16_t a, int16_t b){
    if (abs(a) > abs(b)) return a;
    return b;
}

int16_t lf::follow(){
    #ifdef LF_ACTIVE
        int16_t diff = ls::white.left.value - ls::white.right.value;
        int16_t Adiff = ls::white.left_outer.value - ls::white.left_outer.value;
    if (abs(Adiff) < 25){
        Adiff=0;
    }
    
    
    int16_t vupdate = (diff + Adiff*3)/2;
    
    motor::fwd(motor::motor::B,V_STD + vupdate);
    motor::fwd(motor::motor::A,V_STD - vupdate);
    #endif

    return 0;
}
