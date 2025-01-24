
//
// RPI_COM.cpp
//
// implements the connection protocol to the raspberry pi
//

#include "rpi_com.h"
#include <Wire.h>
#include "Pins.h"
#include <exception>

#define PI_START  0xF0
#define PI_STATUS 0xE0
#define PI_STOP   0x00
#define PI_READ   0x10

rpi::AINotStartedException::AINotStartedException(Ai ai){
    this->ai = ai;
}
const char * rpi::AINotStartedException::what(){
    switch (ai){
        default:
            return "Ai NONE not started!";
        case Ai::VICTIMS:
            return "Ai VICTIMS not started!";
        case Ai::CORNERS:
            return "Ai CORNERS not started!";
        case Ai::EXITS:
            return "Ai EXITS not started!";
    }
}

void rpi::start_ai(Ai ai){
    Wire.beginTransmission(PI_ADDRESS);
    Wire.write(PI_START | ai);
    Wire.endTransmission();
}

void rpi::stop_ai(){
    Wire.beginTransmission(PI_ADDRESS);
    Wire.write(PI_STOP);
    Wire.endTransmission();
}

Victim rpi::get_victim(){
    Wire.beginTransmission(PI_ADDRESS);
    Wire.write(PI_READ | Ai::VICTIMS);
    //Wire.endTransmission(false);
    Wire.requestFrom(PI_ADDRESS, 12);
    Victim victim;
    if (Wire.read() == 0x00) {
        throw AINotStartedException(Ai::VICTIMS);
    }
    else {
        victim.angle = Wire.read();
        victim.dist = Wire.read();
    }
    Wire.endTransmission();
    return victim;

}
uint8_t rpi::status(){
    Wire.beginTransmission(PI_ADDRESS);
    if (!Wire.available()){
        Wire.endTransmission();
        return 0xFF;
    }
    Wire.write(PI_STATUS);
    //Wire.endTransmission(false);
    Wire.requestFrom(PI_ADDRESS, 1);
    uint8_t status = Wire.read();
    Wire.endTransmission();
    return status;
}







