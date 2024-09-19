
#include "claw.h"
#include "shared.h"
#include "Pins.h"
#include "fadc.h"

PWMBus bus(0x40);

void claw::setup(){
    if (!bus.begin()){
        Serial.println("PWM Bus setup ERROR");
        return;
    }

    bus.setFrequency(50);
}

void claw::up(){
    bus.setValue(SERVO1, 0);
}


void claw::down(){
    bus.setValue(SERVO1, 255);
}


