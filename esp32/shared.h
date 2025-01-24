#pragma once

//
// SHARED HEADER
// 
// This header implements some custom base classes (ex. repr) and defines to configure the program before the
// competition
//

#include <string>
#include <Arduino.h>
#include <SPIFFS.h>

// defines (comment out/in for features)
#define DEBUG
//#define LOGGER
//#define BLE
#define MOT_STBY
#define LF_USE_BACK
#define LF_ACTIVE
//#define USE_TIMESCALE
//#define RASP_COMM
//#define LED_TEST
//#define MOT_TEST
#define NO_CLAW
//#define CLAW_TEST
//#define FASTREAD
//#define STORAGE_TEST

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

/*class SerialWriter : public repr {

    File file;
    bool ok = true;
    char* filename;

    public:
        SerialWriter(const char* filename){
            this->filename = filename;
        }
        ~SerialWriter(){
            if(ok) file.close();
        }

        void begin(uint32_t baud){
            file = SPIFFS.open(filename, "w");
            ok = bool(file);
            if(ok){
                file.write("");
                file.close();
                file = SPIFFS.open(filename, "a");
            }
            else file.close();
        }

        void print(char* msg){
            if (ok){
                file.write(msg);
            }
        }
        void println(char* msg)
            if (ok){
                file.write(msg);
                file.write("\n");
            }
        void print(float f){
            if (ok){
                file.write(to_string(f).c_str());
            }
        void println(float){
            if (ok){
                file.write((to_string(f) + "\n").c_str());
            }
        }
        void print(int64_t);
        void println(int64_t);
        void print(uint64_t);
        void println(uint64_t);    

        void end();
    
}*/

#ifndef DEBUG

    //SerialWriter output("out.log");
    #define output Serial
#else
    #define output Serial
#endif



