#pragma once

//
// LIGHTSENSOR HEADER
//
// layouts the lightsensor (for line following) interface.
//

#include <Arduino.h>
#include <cstdint>
#include <initializer_list>
#include <ArduinoJson.h>

#include "Pins.h"
#include "shared.h"

#define LED_DELAY 150 // delay between turning on the LED and reading in us
#define ITER_SKIP 100 // The first x readings are corrupted for no visible reason. Skip these readings when calibrating

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

class lightSensorArray;

class lightSensor{
/*
class that controls a single color light sensor
*/
    friend lightSensorArray;
    public:
        int16_t value = 1; // current (old) calibrated value
        int16_t raw = 0;
        int16_t nvalue = 1; // new calibrated value
                        
        int16_t vmin = 0x7FFF; // calibration minimum
        int16_t vmax = 0; // calibration maximum
        uint8_t sensor_pin;
    
    private:
        uint8_t led_pin;
        uint32_t delay;
        uint8_t pwm;
        
        virtual void led_on();
        virtual void led_off();

    protected:
        void update();
        /*
        update value with nvalue
        */

        void calibrate_turn(int iter);
        /*
        run one calibtration turn
        
        [param iter] iteration index
        */

    public:
        
        lightSensor();
        lightSensor(uint8_t led_pin, uint8_t sensor_pin,uint8_t pwm=0, uint32_t delay=LED_DELAY);
        /*
        create a lightSensor object
        */
        
        #ifndef FASTREAD 
            void read();
            /*
            turn on the led, read out the sensors, turn off the led, map the value and update it
            */
        #else
            void read(lightSensor* prev = nullptr);
            void end_read();

            // see the diffrences between FASTREAD and not-FASTREAD in lightsensor.cpp
        #endif
        
        int16_t get_min();
        int16_t get_max();
};

class lightSensorArray : public repr {
/*
class representing and managing multiple light sensors of the same color
*/  
    public:

        lightSensorArray(lightSensor l_o, lightSensor l, lightSensor r, lightSensor r_o);
        
        lightSensor left_outer, left, right, right_outer;
        
        void calibrate_turn(int iter);
        
        void load(String data);
        string save();

        void read();

        string _str();

        void update();
        /*
        update all associated LightSensor objects
        */
        
};

namespace ls{
/*
namespace that holds all functions for all light sensors
*/

    extern lightSensorArray white, green, red; // front light sensors
    extern lightSensorArray white_b, green_b, red_b; // back light sensors
    extern lightSensorArray* all[6];

    extern int16_t rg_max_l;
    extern int16_t rg_min_l;
    extern int16_t rg_max_r;
    extern int16_t rg_min_r;
    
    extern void setup();
    /*
    configure light sensors
    */
    
    extern const void read(bool update=true);
    /*
    read all lightSensorArrays

    [param update] directly update all values
    */
    
    extern void read(initializer_list<lightSensorArray*> ls, bool update=true);
    /*
    read all given lightSensorArrays.
    
    !raises! NullptrException/LoadProhibited

    [param ls] all light sensors. may not be nullptr!
    [param update] directly update all values
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

    extern const void update();
    /*
    update all LightSensorArrays
    */

    extern void update(initializer_list<lightSensorArray*> ls);
    /*
    update all given lightSensorArrays.
    
    !raises! NullptrException/LoadProhibited

    [param ls] all light sensors. may not be nullptr!
    */
}




