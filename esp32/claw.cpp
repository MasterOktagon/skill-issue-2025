
#include "claw.h"
#include "shared.h"
#include "Pins.h"
#include "fadc.h"

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver bus = Adafruit_PWMServoDriver();

#define SERVOMIN  150 // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  600 // This is the 'maximum' pulse length count (out of 4096)
#define USMIN  600 // This is the rounded 'minimum' microsecond length based on the minimum pulse of 150
#define USMAX  2400 // This is the rounded 'maximum' microsecond length based on the maximum pulse of 600
#define SERVO_FREQ 50 // Analog servos run at ~50 Hz updates

void claw::setup(){

    bus.begin();
    bus.setOscillatorFrequency(27000000);
    bus.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates
}

void claw::up(){
    //bus.wakeup();
    bus.setPWM(SERVO4, 0, 70);
    delay(500);
    //bus.sleep();
}

void claw::down(){
    //bus.wakeup();
    bus.setPWM(SERVO4, 0, 508);
    delay(500);
    //bus.sleep();
}

void claw::half(){
    //bus.wakeup();
    bus.setPWM(SERVO4, 0, 260);
    delay(500);
    //bus.sleep();
}

void claw::open(){
    //bus.wakeup();
    bus.setPWM(SERVO5, 0, 250);
    delay(500);
    //bus.sleep();
}

void claw::wide(){
    //bus.wakeup();
    bus.setPWM(SERVO5, 0, 140);
    delay(500);
    //bus.sleep();
}

void claw::close(){
    //bus.wakeup();
    bus.setPWM(SERVO5, 0, 528);
    delay(500);
    //bus.sleep();
}

void rgb::setValue(Side s, uint8_t r, uint8_t g, uint8_t b){
    ////bus.wakeup();
    if (s & Side::LEFT){
        bus.setPWM(RED_L, 0, 4095 - 8*r);
        bus.setPWM(GREEN_L, 0, 4095 - 8*g);
        bus.setPWM(BLUE_L, 0, 4095 - 8*b);
    }
    if (s & Side::RIGHT){
        bus.setPWM(RED_R, 0, 4095 - 8*r);
        bus.setPWM(GREEN_R, 0, 4095 - 8*g);
        bus.setPWM(BLUE_R, 0, 4095 - 8*b);
    }
    //bus.sleep();
}

void rgb::reset(Side s){
    setValue(s, 0, 0, 0);
}

void storage::unload(Side s){
    //output.println("INFO:j");
    //bus.wakeup();
    if (s == Side::LEFT){
        output.println("INFO: storage::unload LEFT");
        ////bus.wakeup();
        bus.setPWM(SERVO2, 0, 140);
        delay(2000);
        //bus.sleep();
    }
    else if (s == Side::RIGHT){
        output.println("INFO: storage::unload RIGHT");
        ////bus.wakeup();
        bus.setPWM(SERVO2, 0, 528);
        delay(2000);
        //bus.sleep();
    }
    else if (s == Side::NONE){
        output.println("INFO: storage::unload NONE");
        ////bus.wakeup();
        bus.setPWM(SERVO2, 0, 330);
        delay(100);
        //bus.sleep();
    }
    //bus.sleep();
}

void storage::reset(){
    unload(Side::NONE);
}

void storage::divide(Side s, bool wide){
    //bus.wakeup();
    if (s == Side::LEFT){
        output.println("INFO: storage::divide LEFT");
        ////bus.wakeup();
        bus.setPWM(SERVO3, 0, 285 - 100 * wide);
        delay(2000);
        //bus.sleep();
    }
    else if (s == Side::RIGHT){
        output.println("INFO: storage::divide RIGHT");
        ////bus.wakeup();
        bus.setPWM(SERVO3, 0, 448 + 80 * wide);
        delay(2000);
        //bus.sleep();
    }
    else {
        output.println("WARNING: unknown/unimplemented storage::divide Side");
    }
    //bus.sleep();
}

void rgb::highbeam(bool status){
    //bus.wakeup();
    bus.setPWM(SERVO1, 0, 2048*int(status));
}


