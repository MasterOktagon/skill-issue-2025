
#pragma once

#include <Arduino.h>
#include "Pins.h"
#include "MPU6050_light.h"
#include "Wire.h"

extern MPU6050 mpu; // mpu object. DO NOT USE DIRECTLY

namespace gyro{
/*
namespace that hols all fucntions working with the gyro sensor
*/
    extern bool init();
    /*
    Setup and initialize the MPU6050 


    [return] false if an error occured (most likely due to a wrong I2C cable)
    */

    extern void reset();
    /*
    reset rotations
    */

    extern void update();
    /*
    update values. should be called often
    */

    extern float x, y, z; // gyro values on different axes

    extern float XAccel;
}
