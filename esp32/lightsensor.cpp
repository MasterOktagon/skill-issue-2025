#include "adc.h"

#include <Arduino.h>
#include <cstdint>

#include "lightsensor.h"
#include "shiftregister.h"


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




