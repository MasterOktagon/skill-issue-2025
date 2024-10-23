#pragma once

//
// SHARED HEADER
// 
// This header implements some custom base classes (ex. repr) and defines to configure the program before the
// competition
//

#include <string>
#include <Arduino.h>

// defines (comment out/in for features)
#define DEBUG
//#define BLE
//#define MOT_STBY
//#define LF_USE_BACK
#define LF_ACTIVE
#define USE_TIMESCALE
//#define RASP_COMM
//#define LED_TEST
#define CLAW_TEST
//#define FASTREAD

#ifndef DEBUG
    //PrintWriter output;
    #define output Serial
#else
    #define output Serial
#endif

// define the 'signal' type
template <typename T, typename ... K>
using fsignal = T (*) (K ... );

using namespace std;

class repr{
    /*
    a 'represent' class for showing everything
    */
    public:
    virtual string _str();
};


inline const char* str(repr r){
    return r._str().c_str();
}

enum Side {
    NONE = 0,
    LEFT = 3,
    RIGHT = 12,
    BOTH = 15
};

#define PWMBus Bonezegei_PCA9685 // no one wants to write so much

