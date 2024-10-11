
#include "claw.h"
#include "shared.h"
#include "Pins.h"
#include "fadc.h"

PWMBus bus(BUS_ADDRES);

void claw::setup(){
    if (!bus.begin()){
        output.println("PWM Bus setup ERROR");
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

void rgb::setValue(Side s, uint8_t r, uint8_t g, uint8_t b){
    if (s & Side::LEFT){
        bus.setValue(RED_L, r);
        bus.setValue(GREEN_L, g);
        bus.setValue(BLUE_L, b);
    }
    if (s & Side::RIGHT){
        bus.setValue(RED_R, r);
        bus.setValue(GREEN_R, g);
        bus.setValue(BLUE_R, b);
    }
}


