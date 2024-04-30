#include "esp32-hal-adc.h"
#include "esp32-hal.h"


#include "adc.h"

namespace adc{
    CD74HC4067 multi(S0, S1, S2, S3);
    uint8_t current_channel = 0; // the current channel
}

int16_t adc::read(uint8_t pin){
    if(pin != current_channel){ // only wait if the pin isn't already channeled 
        multi.channel(pin);
        delayMicroseconds(10);
        current_channel = pin;
    }
    return analogRead(ADC_MULTI);
}
