
#include <Arduino.h>
#include <esp32-hal-adc.h>
#include <hal/adc_hal.h>
#include <driver/periph_ctrl.h>
#include <driver/adc.h>
//#include <esp_adc_cal.h>
#include <cstdint>
#include <mutex>

#include "fadc.h"
#include "Pins.h"
#include "shared.h"

//#define USE_MTX // use mutexes

#define ADC_BUSY() !((bool)SENS.sar_meas1_ctrl2.meas1_done_sar)

using namespace std;

const int8_t gpio_to_channel[21] = {
    -1,
    ADC1_GPIO1_CHANNEL, ADC1_GPIO2_CHANNEL, ADC1_GPIO3_CHANNEL, ADC1_GPIO4_CHANNEL, ADC1_GPIO5_CHANNEL,
    ADC1_GPIO6_CHANNEL, ADC1_GPIO7_CHANNEL, ADC1_GPIO8_CHANNEL, ADC1_GPIO9_CHANNEL, ADC1_GPIO10_CHANNEL,

    ADC2_GPIO11_CHANNEL, ADC2_GPIO12_CHANNEL, ADC2_GPIO13_CHANNEL, ADC2_GPIO14_CHANNEL, ADC2_GPIO15_CHANNEL,
    ADC2_GPIO16_CHANNEL, ADC2_GPIO17_CHANNEL, ADC2_GPIO18_CHANNEL, ADC2_GPIO19_CHANNEL, ADC2_GPIO20_CHANNEL,
    };


#ifdef USE_MTX
    mutex adcread;
#endif

void fadc::start(uint8_t pin){
    #ifdef USE_MTX
         adcread.lock();
    #endif
    #ifdef DEBUG
        if (pin < 1 || pin > 20){ output.print("Pin has no channel: "); output.println(pin); return; }
    #endif

    SENS.sar_meas1_ctrl2.sar1_en_pad = (1 << gpio_to_channel[pin]);
    SENS.sar_meas1_ctrl2.meas1_start_sar = 0;
    SENS.sar_meas1_ctrl2.meas1_start_sar = 1;

    #ifdef USE_MTX
        adcread.unlock();
    #endif
}

bool fadc::busy(){
    return ADC_BUSY();
}

uint16_t fadc::value(){
    return HAL_FORCE_READ_U32_REG_FIELD(SENS.sar_meas1_ctrl2, meas1_data_sar);
}

uint16_t fadc::read(uint8_t pin){
    start(pin);
    while(busy());
    return value();
}

void fadc::begin(){
    
    // Initialize ADC using built-ins
    analogReadResolution(FADC_RESOLUTION);
    analogSetAttenuation(FADC_ATTEN);
    
    // Initialize pins using built-ins
    for (uint i = 1; i < 21; i++){
        analogSetPinAttenuation(i, FADC_ATTEN);
        analogRead(i);
    }

    // Enable ADC
    //periph_module_enable(PERIPH_SARADC_MODULE);
    //adc_power_acquire();
}








