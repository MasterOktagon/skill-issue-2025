#pragma once

//
// SHARED HEADER
//

#include <string>

// defines (comment out/in for features)
//#define DEBUG
//#define BLE
//#define MOT_STBY
//#define LF_USE_BACK
#define LF_ACTIVE
#define USE_TIMESCALE
//#define RASP_COMM
//#define LED_TEST

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

/*
inline string str(repr r){
    return r._str();
}*/

enum Side {
    NONE = 0,
    LEFT = 3,
    RIGHT = 12,
    BOTH = 15
};
