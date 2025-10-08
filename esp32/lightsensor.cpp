#include "esp32-hal-gpio.h"
#include "Pins.h"

//
// LIGHTSENSOR.cpp
//
// implements the lightsensor interface
//

#include <Arduino.h>
#include <cstdint>
#include <initializer_list>
#include <string>
#include <ArduinoJson.h>
#include <SPIFFS.h>

#include "lightsensor.h"
#include "shiftregister.h"
#include "fadc.h"
#include "shared.h"

// make this independent of "shared.h"
#ifndef output
    #define output Serial
#endif

using namespace std;

uint8_t last_led = PT_GREEN; // which led was turned on lastly.

void fs::setup(){
    /*
    call fs::setup in your setup routine. requires SPIFFS to work
    */
    output.println("INFO: mounting SPIFFS");
    if (!SPIFFS.begin(true)) { // NOTE: this will format your device to use SPIFFS
        output.println("ERROR: An Error has occurred while mounting SPIFFS");
    }
}

lightSensor::lightSensor(uint8_t led_pin, uint8_t sensor_pin, uint8_t pwm, uint32_t delay){
    this->led_pin = led_pin;
    this->sensor_pin = sensor_pin;
    this->delay = delay;
    this->pwm = pwm;
}

lightSensor::lightSensor(){
    led_pin = 0;
    sensor_pin = 0;
}

void lightSensor::led_on(){
    //if (led_pin != last_led){
        //digitalWrite(last_led, LOW);
        //delayMicroseconds(delay);
        if (pwm == 0)
            digitalWrite(led_pin, HIGH);
        else {
            analogWrite(led_pin, pwm);
        }
        delayMicroseconds(delay); // Delay because your LED needs time to turn on

        //last_led = led_pin;
    //}
}

void lightSensor::led_off(){
    digitalWrite(led_pin, LOW); // we do not have to wait here because it can turn off while the next
                                // LED is turned on
    delayMicroseconds(delay);
}

#ifndef FASTREAD
    // Default reading function (uses arduino analogRead)

    void lightSensor::read(){
        led_on();
        // read value and map it to a range between 0 and 100
        raw = analogRead(sensor_pin);
        int a = ((raw - vmin) * 100);
        nvalue = int16_t(a / (vmax - vmin));
        led_off();
    }
#else
    // The FADC version
    // FADC supports asyncronous reading so we map while the next lightSensor
    // reads its raw values

    void lightSensor::read(lightSensor* prev){
        led_on();

        fadc::start(sensor_pin);
            if (prev != nullptr){prev->end_read();}
        while(fadc::busy()){};
        raw = fadc::value();
        led_off();

    }

    void lightSensor::end_read(){
        int a = ((raw - vmin) * 100);
        nvalue = int16_t(a / (vmax - vmin));
    }
#endif

void lightSensor::update(){
    /*
    switch the new value with the old value.
    this allows other threads to acces 'value' while reading (avoiding race conditions)
    this function should be called after thread::join()
    */
    value = nvalue;
}

void lightSensor::calibrate_turn(int iter){
    /*
    single calibration turn.
    A value is read out and the min/max values are stored to later map the
    read-out values between them
    */
    led_on();

    //output.println(sensor_pin);
    int16_t val = analogRead(sensor_pin); // read light sensor value
    value = val;
    if (iter > ITER_SKIP){
        // update min/max values
        vmax = max(vmax, val);
        vmin = min(vmin, val);
    }

    led_off();
}

int16_t lightSensor::get_max(){return vmax;}
int16_t lightSensor::get_min(){return vmin;}

/*
A lightSensorArray is just a container that has 4 lightSensors of one color.
It also adds some shared functions for convinience and to clean up the main loop code.
*/

lightSensorArray::lightSensorArray(lightSensor l_o, lightSensor l, lightSensor r, lightSensor r_o){
    left_outer  = l_o;
    left        = l;
    right       = r;
    right_outer = r_o;
}

void lightSensorArray::calibrate_turn(int iter){
    left_outer.calibrate_turn(iter);
    left.calibrate_turn(iter);
    right.calibrate_turn(iter);
    right_outer.calibrate_turn(iter);
}

string lightSensorArray::_str(){
    string s = "INFO: LightSensorArray:\t{L_1 ";
    s += to_string(left_outer.get_min())  + "/" + to_string(left_outer.get_max())  + " \tL_0 ";
    s += to_string(left.get_min())        + "/" + to_string(left.get_max())        + " \tR_0 ";
    s += to_string(right.get_min())       + "/" + to_string(right.get_max())       + " \tR_1 ";
    s += to_string(right_outer.get_min()) + "/" + to_string(right_outer.get_max()) + " \t}" ;

    return s;
}

/*
Saving is done using JSON files since they are very human-readable
*/

string lightSensorArray::save(){
    JsonDocument data;
    data["left_outer"]["min"] = left_outer.vmin;
    data["left_outer"]["max"] = left_outer.vmax;

    data["left"]["min"] = left.vmin;
    data["left"]["max"] = left.vmax;

    data["right"]["min"] = right.vmin;
    data["right"]["max"] = right.vmax;

    data["right_outer"]["min"] = right_outer.vmin;
    data["right_outer"]["max"] = right_outer.vmax;

    string s;
    serializeJson(data, s);
    return s;
}

void lightSensorArray::read(){
    #ifdef FASTREAD
        left_outer.read();
        left.read(&left_outer);
        right.read(&left);
        right_outer.read(&right);
        right_outer.end_read(); // dont forget to call end_read on your last lightSensor when in FASTREAD mode
    #else
        left_outer.read();
        left.read();
        right.read();
        right_outer.read();
    #endif
}

void lightSensorArray::update(){
    left_outer.update();
    left.update();
    right.update();
    right_outer.update();
}

void lightSensorArray::load(String data){
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, data);

    if (error) {
        output.print(F("ERROR: local deserializeJson() failed: "));
        output.println(error.f_str());
        return;
    }

    left_outer.vmin = doc["left_outer"]["min"];
    left_outer.vmax = doc["left_outer"]["max"];

    left.vmin = doc["left"]["min"];
    left.vmax = doc["left"]["max"];

    right.vmin = doc["right"]["min"];
    right.vmax = doc["right"]["max"];

    right_outer.vmin = doc["right_outer"]["min"];
    right_outer.vmax = doc["right_outer"]["max"];
}


namespace ls{
    // This constructs all needed LightSensor objects
    // Back Light sensor bar objects are postfixed with an '_b'

    lightSensorArray white(
        lightSensor(PT_WHITE_L, PT_L_1),
        lightSensor(PT_WHITE_L, PT_L_0),
        lightSensor(PT_WHITE_R, PT_R_0),
        lightSensor(PT_WHITE_R, PT_R_1)
    );
    lightSensorArray white_b(
        lightSensor(PT_WHITE_L, PT_L_3),
        lightSensor(PT_WHITE_L, PT_L_2),
        lightSensor(PT_WHITE_R, PT_R_2),
        lightSensor(PT_WHITE_R, PT_R_3)
    );

    lightSensorArray green(
        lightSensor(PT_GREEN, PT_L_1),
        lightSensor(PT_GREEN, PT_L_0),
        lightSensor(PT_GREEN, PT_R_0),
        lightSensor(PT_GREEN, PT_R_1)
    );

    lightSensorArray green_b(
        lightSensor(PT_GREEN, PT_L_3),
        lightSensor(PT_GREEN, PT_L_2),
        lightSensor(PT_GREEN, PT_R_2),
        lightSensor(PT_GREEN, PT_R_3)
    );

    lightSensorArray red(
        lightSensor(PT_RED, PT_L_1),
        lightSensor(PT_RED, PT_L_0),
        lightSensor(PT_RED, PT_R_0),
        lightSensor(PT_RED, PT_R_1)
    );
    lightSensorArray red_b(
        lightSensor(PT_RED, PT_L_3),
        lightSensor(PT_RED, PT_L_2),
        lightSensor(PT_RED, PT_R_2),
        lightSensor(PT_RED, PT_R_3)
    );

    lightSensorArray* all[6] = {&white, &green, &red, &white_b, &green_b, &red_b};
}


const void ls::read(bool doupdate){
    white.read();
    //white_b.read();

    red.read();
    red_b.read();
    
    green.read();
    green_b.read();

    if (doupdate){update();}
}

const void ls::update(){
    white.update();
    red.update();
    green.update();

    white_b.update();
    red_b.update();
    green_b.update();
}

void ls::read(initializer_list<lightSensorArray*> ls, bool doupdate){
    for (lightSensorArray* l : ls){
        l->read();
        if (doupdate){l->update();}
    }
}

void ls::update(initializer_list<lightSensorArray*> ls){
    for (lightSensorArray* l : ls){
        l->update();
    }
}

int16_t ls::rg_min_l = 0x7FFF;
int16_t ls::rg_max_l = 0;
int16_t ls::rg_min_r = 0x7FFF;
int16_t ls::rg_max_r = 0;

void ls::calibrate(uint16_t iterations, uint16_t delay_ms){
    output.print("Calibrating...");
    for(uint16_t i = 0; i < iterations; i++){
        for(uint8_t ls = 0; ls < 6; ls++){
            if (all[ls] != nullptr){
                all[ls]->calibrate_turn(i);
            }
        }
        delay(delay_ms);
    }
    rg_max_l = max(rg_max_l, (int16_t(green.left.value -  red.left.value)));
    rg_min_l = min(rg_min_l, (int16_t(green.left.value -  red.left.value)));
    rg_max_r = max(rg_max_r, (int16_t(green.right.value - red.right.value)));
    rg_min_r = min(rg_min_r, (int16_t(green.right.value - red.right.value)));
    output.println("Succes!");
}

void ls::save(){
    File file = SPIFFS.open("/calibration.json","w");
    if (!file){
      output.println("ERROR: opening write file!");
      file.close();
      return;
    }
    
    JsonDocument doc;
    doc["white"] = white.save();
    doc["green"] = green.save();
    doc["red"]   = red.save();

    doc["white_b"] = white_b.save();
    doc["green_b"] = green_b.save();
    doc["red_b"]   = red_b.save();

    doc["rg_max_l"] = rg_max_l;
    doc["rg_min_l"] = rg_min_l;
    doc["rg_max_r"] = rg_max_r;
    doc["rg_min_r"] = rg_min_r;

    serializeJson(doc, file);

    file.close();
}

void ls::load(){
    output.println("Loading from file...");
    File f = SPIFFS.open("/calibration.json","r");

    if (!f){
        output.println("ERROR: opening read file!");
        f.close();
        return;
    }

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, f.readString());

    if (error) {
        output.print(F("ERROR: global deserializeJson() failed: "));
        output.println(error.f_str());
        return;
    }
    white.load(doc["white"]);
    green.load(doc["green"]);
    red.load(doc["red"]);

    white_b.load(doc["white_b"]);
    green_b.load(doc["green_b"]);
    red_b.load(doc["red_b"]);

    rg_max_l = doc["rg_max_l"];
    rg_min_l = doc["rg_min_l"];
    rg_max_r = doc["rg_max_r"];
    rg_min_r = doc["rg_min_r"];

    f.close();
}

// setup sets up the pins needed to read out the light values

void ls::setup(){
    
    pinMode(PT_WHITE_L, OUTPUT);
    pinMode(PT_WHITE_REF, OUTPUT);
    pinMode(PT_WHITE_R, OUTPUT);
    pinMode(PT_RED, OUTPUT);
    pinMode(PT_GREEN, OUTPUT);

    pinMode(PT_REF_L, INPUT);
    pinMode(PT_L_1, INPUT);
    pinMode(PT_L_0, INPUT);
    pinMode(PT_R_0, INPUT);
    pinMode(PT_R_1, INPUT);
    pinMode(PT_REF_R, INPUT);

    pinMode(PT_L_3, INPUT);
    pinMode(PT_L_2, INPUT);
    pinMode(PT_R_2, INPUT);
    pinMode(PT_R_3, INPUT);
}


