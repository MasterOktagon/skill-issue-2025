
#include "claw.h"
#include "shared.h"
#include "Pins.h"
#include "fadc.h"

PWMBus bus(BUS_ADDRESS);

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
    delay(2000);
}

void claw::half(){
    bus.setValue(SERVO1, 128);
    delay(2000);
}

void claw::open(){
    bus.setValue(SERVO2, 128);
}

void claw::wide(){
    bus.setValue(SERVO2, 0);
}

void claw::close(){
    bus.setValue(SERVO2, 255);
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


