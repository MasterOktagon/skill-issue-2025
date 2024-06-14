
#pragma once

#include <Arduino.h>
#include <cstdint>
#include <initializer_list>
#include <ArduinoJson.h>

#include "Pins.h"
#include "shared.h"

using namespace std;

namespace fs{
/*
Holds functions general to the filesystem
*/
    extern void setup();
    /*
    Start SPIFFS filesystem
    */
}


struct lsData {
/*
Holds data for one light sensor
*/
    int16_t min = 0x7FFF;
    int16_t max = 1;
    int16_t raw = 0; // not sure if this is going to be used
    int16_t value = 0;
    uint8_t adc_pin;
};

class lsBase : public repr{
/*
Base class for all light sensors.
*/
    public:

        virtual void calibrate_turn(uint16_t i);
        /*
        one calibration iteration

        [param i] number of the iteration
        */

        virtual void read();
        /*
        update the light values
        */
        virtual string _str();

        virtual string save();

        virtual void load(String data);

};

class lightSensorArray : public lsBase{
/*
class that represents all light sensors in a light sensor bar V2 for one led color
*/
    private:
        uint8_t led_pin;
        void led_on();
        void led_off();

    public:

        lsData left_outer;
        lsData left;
        lsData center;
        lsData right;
        lsData right_outer;

        lightSensorArray(uint8_t led_pin,
            const uint8_t left_outer=ADC_PT_L_1,
            const uint8_t left=ADC_PT_L_0,
            const uint8_t center=ADC_PT_M,
            const uint8_t right=ADC_PT_R_0,
            const uint8_t right_outer=ADC_PT_R_1); //TODO: configure adc ports
        /*
        create a lightSensorArray object
        */
        void calibrate_turn(uint16_t i);
        void read();
        string _str();
        string save();
        void load(String data);
};

class rawSensor : public lsBase{
    uint8_t led_pin;
    void led_on();
    void led_off();

    public:
        lsData data;
        void calibrate_turn(uint16_t i);
        void read();
};

namespace ls{
/*
namespace that holds all functions for all light sensors
*/

    extern lightSensorArray white, green, red; // front light sensors
    #if (BOARD_REVISION > 1)
      extern lightSensorArray back; // back light sensors
    #endif

    extern rawSensor ref_l, ref_r; // reflective sensors

    extern const void read();
    /*
    read all lightSensorArrays
    */

    extern void read(initializer_list<lightSensorArray*> ls);
    /*
    read all given lightSensorArrays.
    
    !raises! NullptrException/LoadProhibited

    [param ls] all light sensors. may not be nullptr!
    */

    extern void calibrate(uint16_t iterations, uint16_t delay_ms=0);
    /*
    calibrate all light Sensors

    [param iterations] amount to calibrate
    [param delay_ms] delay between measurements in ms
    */

    extern void save();
    /*
    save all lightsensor data to a file
    */

    extern void load();
    /*
    load all lightsensor data from a file
    */

}

