#include <iterator>
#include "adc.h"

#include <Arduino.h>
#include <cstdint>
#include <initializer_list>
#include <string>

#include "lightsensor.h"
#include "shiftregister.h"

using namespace std;

lightSensorArray::lightSensorArray(uint8_t led_pin,
            const uint8_t left_outer,
            const uint8_t left,
            const uint8_t center,
            const uint8_t right,
            const uint8_t right_outer){
            
    this->led_pin = led_pin;
    Serial.println(left_outer);
    this->left_outer.adc_pin  = ADC_PT_L_0;
    this->left.adc_pin        = left;
    this->center.adc_pin      = center;
    this->right.adc_pin       = right;
    this->right_outer.adc_pin = right_outer;
}

void lightSensorArray::led_on(){
    shiftregister::set(led_pin, HIGH);
    delayMicroseconds(10);
}

void lightSensorArray::led_off(){
    shiftregister::set(led_pin, LOW, false);
}


#define cal(side)                            \
    current_value = adc::read(side.adc_pin); \
    side.min = min(side.min, current_value); \
    side.max = max(side.max, current_value)

void lightSensorArray::calibrate_turn(uint16_t i){
    led_on();
    int16_t current_value;
    cal(left_outer);
    cal(left);
    cal(center);
    cal(right);
    cal(right_outer);
    
    led_off();
}

inline void read_side(lsData& side){
    
    int16_t a = (adc::read(side.adc_pin) * 100);
    side.value = int16_t(a / (side.max - side.min));

}

void lightSensorArray::read(){
    
    led_on();
    read_side(left_outer);
    read_side(left);
    read_side(center);
    read_side(right);
    read_side(right_outer);
    led_off();
}

string lightSensorArray::_str(){
    string s = "LightSensorArray:\t{L_1 ";
    s += to_string(left_outer.min)  + "/" + to_string(left_outer.max)  + " \tL_0";
    s += to_string(left.min)        + "/" + to_string(left.max)        + " \tM ";
    s += to_string(center.min)      + "/" + to_string(center.max)      + " \tR_0 ";
    s += to_string(right.min)       + "/" + to_string(right.max)       + " \tR_1 ";
    s += to_string(right_outer.min) + "/" + to_string(right_outer.max) + " \t}" ;

    return s;
}

namespace ls{
    lightSensorArray white(SR_PT_WHITE);
    lightSensorArray green(SR_PT_GREEN);
    lightSensorArray red(SR_PT_RED);
    #if (BOARD_REVISION > 1)
        lightSensorArray back(SR_PT_WHITE, ADC_PT_BACK_L_1, ADC_PT_BACK_L_0, ADC_PT_BACK_L_0, ADC_PT_BACK_R_0, ADC_PT_BACK_R_1);
        lightSensorArray* all[4] = {&white, &green, &red, &back};
    #else 
        #define back nullptr
        lightSensorArray* all[4] = {&white, &green, &red, back};
    #endif


}


void ls::read(){
    white.read();
    red.read();
    green.read();
    #if (BOARD_REVISION > 1)
        back.read();
    #endif
}

void ls::read(initializer_list<lightSensorArray*> ls){
    for (lightSensorArray* l : ls){
        l->read();
    }
}

void ls::calibrate(uint16_t iterations, uint16_t delay_ms){
    Serial.print("Calibrating...");
    for(uint16_t i = 0; i < iterations; i++){
        for(uint8_t ls = 0; ls < 4; ls++){
            if (all[ls] != nullptr){
                all[ls]->calibrate_turn(i);
            }
        }
        delay(delay_ms);
    }
    Serial.println("Succes!");
}





