
//
// GYRO.cpp
//
// implementation of the gyro interface for the mpu6050
//

#define byte uint8_t
#include <sys/_types.h>
#include <sys/_stdint.h>
#include <esp32-hal.h>
#include <Arduino.h>
#include <MPU6050_light.h>
#include <Wire.h>
#include <cmath>

#include "Pins.h"
#include "gyro.h"
#include "shared.h"


using namespace std;

MPU6050 mpu(Wire);
float gyro::x, gyro::y, gyro::z; // gyro values on different axes

namespace gyro{
    unsigned long timestamp;   // [PRIVATE] timestamp for updates of the gyro
}

bool gyro::init() {
/*
Setup and initialize the MPU6050 
*/
    byte status = mpu.begin();
    if(status != 0){
        output.println(F("ERROR: Fail to detect MPU6050!"));
        return false;
    }
    else{
        delay(500);
        mpu.calcOffsets();
    }
    return true;
}

void gyro::update() {
/*
Reads out the values of the gyro sensor via I2C
*/

    mpu.update();
    if (timestamp == 0){ // -> first measurement
        timestamp = micros();
        return;
    }
    unsigned long t = micros(); // time difference
    unsigned long diff = t - timestamp;
    timestamp = t;
    
    // calculate new rotation
    x += round((mpu.getGyroX() * diff) / 1000000.0);
    y += (mpu.getGyroY() * diff) / 1000000.0;
    z += (mpu.getGyroZ() * diff) / 1000000.0;
}

void gyro::reset() {
  //mpu.calcOffsets();
  timestamp = 0;
  x = 0;
  y = 0;
  z = 0;
}
