#include "gyro.h"
#include "color.h"

#include <cstdint>
#include <Arduino.h>

#include "motor.h"
#include "shiftregister.h"

#define MOT_CORRECTION -2

bool button_failure = false; // wether buttons have a failure

int16_t operator ""cm (unsigned long long int a){
    return (int16_t) (a * (V_STD/120) * 1000/18);
}

void motor::stop(bool hard){
    // stop the motors
    digitalWrite(PWMA1, LOW);
    digitalWrite(PWMB1, LOW);
    digitalWrite(PWMA2, LOW);
    digitalWrite(PWMB2, LOW);

    if (hard){return;}

    // clean up
    shiftregister::set(SR_AIN1_F, LOW, false);
    shiftregister::set(SR_AIN1_R, LOW, false);
    shiftregister::set(SR_AIN2_F, LOW, false);
    shiftregister::set(SR_AIN2_R, LOW, false);
    shiftregister::set(SR_BIN1_F, LOW, false);
    shiftregister::set(SR_BIN1_R, LOW, false);
    shiftregister::set(SR_BIN2_R, LOW, false);
    shiftregister::set(SR_BIN2_F, LOW);
}

void motor::stop(motor m, bool hard){

    if (m & motor::A1){
        digitalWrite(PWMA1, LOW);
        if (!hard){
            shiftregister::set(SR_AIN1_F, LOW, false);
            shiftregister::set(SR_AIN1_R, LOW, false);
        }
    }
    if (m & motor::B1){
        digitalWrite(PWMB1, LOW);
        if (!hard){
            shiftregister::set(SR_BIN1_F, LOW, false);
            shiftregister::set(SR_BIN1_R, LOW, false);
        }
    }
    if (m & motor::A2){
        digitalWrite(PWMA2, LOW);
        if (!hard){
            shiftregister::set(SR_AIN2_F, LOW, false);
            shiftregister::set(SR_AIN2_R, LOW, false);
        }
    }
    if (m & motor::B2){
        digitalWrite(PWMB2, LOW);
        if (!hard){
            shiftregister::set(SR_BIN2_F, LOW, false);
            shiftregister::set(SR_BIN2_R, LOW, false);
        }
    }

    shiftregister::shift();
}

/*void motor::hardstop(){
  // reerse all motors
  stop(true);
  rev(25);
  stop();
}*/

void motor::hardstop(){
    stop(true);
}

void motor::fwd(motor m, int16_t v){
  if (v > 200) v = 200;
  else if (v < -180) v = -180;

  // control the motors
  bool rev = v < 0; // wether to reverse *IN1 and *IN2
  v = abs(v);
  if (m & motor::A1){
    shiftregister::set(SR_AIN1_F,  rev, false);
    shiftregister::set(SR_AIN1_R, !rev, false);
    analogWrite(PWMA1, v-MOT_CORRECTION);
  }
  if (m & motor::A2){
    shiftregister::set(SR_AIN2_F,  rev, false);
    shiftregister::set(SR_AIN2_R, !rev, false);
    analogWrite(PWMA2, v-MOT_CORRECTION);
  }
  if (m & motor::B1){
    shiftregister::set(SR_BIN1_F,  rev, false);
    shiftregister::set(SR_BIN1_R, !rev, false);
    analogWrite(PWMB1, v+MOT_CORRECTION);
  }
  if (m & motor::B2){
    shiftregister::set(SR_BIN2_F,  rev, false);
    shiftregister::set(SR_BIN2_R, !rev, false);
    analogWrite(PWMB2, v+MOT_CORRECTION);
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
    #ifndef MOT_STBY
        if (angle == 0){
            return;
        }
        if (reset_gyro){
            stop();
            mpu.calcGyroOffsets();
            gyro::reset();
        }
        int sign = 1;
        if (angle < 0){
            sign = -1;
        }
        turn(v * -sign);
        while (abs(angle) > abs(gyro::z)){
            gyro::update();
            Serial.print(abs(gyro::z));
            Serial.print("\t");
            Serial.println(abs(angle));
        }
        stop();
        gyro::reset();
    #endif
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
    //uint32_t timestamp = millis();
    timer t(time);
    fwd(motor::AB, v);
    do {
        ls::read();
        color::update(colors);
    } while (/*millis() - timestamp < time*/ !t.expired());
    stop();
}
