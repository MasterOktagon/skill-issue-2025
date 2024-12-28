
#include <Arduino.h>
#include "shiftregister.h"
#ifndef output
    #define output Serial
#endif

static uint64_t shiftregister_bits; // the current shiftregister pins values are stored here. max 64 ports
static bool updated; // if the current bits are flushed

bool shiftregister::setup(){
    pinMode(SHCP, OUTPUT);
    pinMode(STCP, OUTPUT);
    pinMode(DS, OUTPUT);
    return true; // might change in future
}

void shiftregister::set(uint8_t pin, bool state, bool flush){
    
    uint64_t setbit = 1 << pin; // get the bit for the shiftregister_bits position
    // update the bits
    if (state){
        shiftregister_bits |= setbit;
    }
    else{
        shiftregister_bits &= ~setbit;
    }
    if (!flush){
        updated = false;
        return;
    }
    
    // push the shiftregister
    digitalWrite(STCP, LOW);
    for(uint8_t i=(SR_PORTS); i > 0; i--){
        digitalWrite(SHCP, LOW);
        digitalWrite(DS, shiftregister_bits & (1 << (i-1))? 1 : 0);
        digitalWrite(SHCP, HIGH);
    
    }
    digitalWrite(STCP, HIGH);
    updated = true;
}

bool shiftregister::is_flushed(){
    return updated;
}

void shiftregister::reset(){
    shiftregister_bits = 0;
    //for(int i = 9; i < 6 + 9; i++){
    //  set(i, HIGH, false);
    //}
    set(SR_STBY1, HIGH);
    set(SR_STBY2, HIGH);
    output.println("INFO: SR Reset succesfull");
}

bool shiftregister::get(uint8_t pin){
    return shiftregister_bits & (1 << pin);
}

void shiftregister::shift(){
    // push the shiftregister
    digitalWrite(STCP, LOW);
    for(uint8_t i=(SR_PORTS); i > 0; i--){
        digitalWrite(SHCP, LOW);
        digitalWrite(DS, shiftregister_bits & (1 << (i-1))? 1 : 0);
        digitalWrite(SHCP, HIGH);
    
    }
    digitalWrite(STCP, HIGH);
    updated = true;
}

