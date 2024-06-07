

#include <esp32-hal-adc.h>
#include <esp32-hal.h>
#include "adc.h"

namespace adc{
    CD74HC4067 multi(S0, S1, S2, S3); // adc data structure
}

void adc::setup(){
}

int16_t adc::read(uint8_t pin){ 
    multi.channel(pin); 
    delayMicroseconds(1);
    return analogRead(ADC_MULTI);
}
