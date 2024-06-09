#include "gyro.h"
#include "color.h"

#include <cstdint>
#include <Arduino.h>

#include "motor.h"
#include "shiftregister.h"

#if (BOARD_REVISION > 2)
    #warning "Controlling single motors is currently not supported"
#endif

void motor::stop(){
    // stop the motors
    digitalWrite(PWMA, LOW);
    digitalWrite(PWMB, LOW);

    // clean up
    shiftregister::set(SR_AIN1, LOW, false);
    shiftregister::set(SR_AIN2, LOW, false);
    shiftregister::set(SR_BIN1, LOW, false);
    shiftregister::set(SR_BIN2, LOW);
}

void motor::stop(motor m){

    if (m & motor::A){
        digitalWrite(PWMA, LOW);
        shiftregister::set(SR_AIN1, LOW, false);
        shiftregister::set(SR_AIN2, LOW, false);
    }
    else if (m & motor::B){
        digitalWrite(PWMB, LOW);
        shiftregister::set(SR_BIN1, LOW, false);
        shiftregister::set(SR_BIN2, LOW, false);
    }
}

void motor::hardstop(){
  // reerse all motors
  shiftregister::set(SR_AIN1, !shiftregister::get(SR_AIN1), false);
  shiftregister::set(SR_AIN2, !shiftregister::get(SR_AIN2), false);
  shiftregister::set(SR_BIN1, !shiftregister::get(SR_BIN1), false);
  shiftregister::set(SR_BIN2, !shiftregister::get(SR_BIN2));
  
  delay(25);
  stop();
}


void motor::fwd(motor m, int16_t v){
  if (v == 0){
    stop(m); return;
  }
  // clamp the speed
  v = min(max(v, int16_t(-255)), int16_t(255));

  // control the motors
  bool rev = v < 0; // wether to reverse *IN1 and *IN2
  v = abs(v);
  if (m & motor::A){
    shiftregister::set(SR_AIN1,  rev, false);
    shiftregister::set(SR_AIN2, !rev, false);
    analogWrite(PWMA, v);
  }
  if (m & motor::B){
    shiftregister::set(SR_BIN1,  rev, false);
    shiftregister::set(SR_BIN2, !rev, false);
    analogWrite(PWMB, v);
  }

  shiftregister::shift();
}

void motor::rev(motor m, int16_t v){
  fwd(m, -v);
}

void motor::fwd(uint32_t time){
  fwd(motor::AB, V_STD);
  delay(time);
  stop();
}

void motor::rev(uint32_t time){
  rev(motor::AB, V_STD);
  delay(time);
  stop();
}

void motor::turn(int16_t v){
  fwd(motor::B, v);
  rev(motor::A, v);
}

void motor::gyro(int16_t angle, uint16_t v, bool reset_gyro){
    if (angle == 0){
        return;
    }
    if (reset_gyro){
        gyro::reset();
    }
    int sign = 1;
    if (angle < 0){
        sign = -1;
    }
    turn(v * -sign);
    while (abs(angle) > abs(gyro::z)){
        gyro::update();
        Serial.println(gyro::z);
    }
    stop();
}

bool motor::sensor_fwd(int16_t v, uint32_t time, initializer_list<color::color*> colors, Side side){
    uint32_t timestamp = millis();
    bool in_loop = true;
    fwd(motor::AB, v);
    do {
        if (millis() - timestamp > time){
            stop(); return false;
        }
        in_loop = true;
        ls::read();
        color::update(colors);
        for (color::color* c : colors){
            in_loop = in_loop && !(c->get() == side);
        }

    } while (in_loop);
    stop();
    return true;
}

void motor::read_fwd(int16_t v, uint32_t time, initializer_list<color::color*> colors){
    uint32_t timestamp = millis();
    fwd(motor::AB, v);
    do {
        ls::read();
        color::update(colors);
    } while (millis() - timestamp < time);
    stop();
}
