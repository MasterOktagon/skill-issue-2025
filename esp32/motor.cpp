
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
  fwd(motor::A, v);
  rev(motor::B, v);
}
