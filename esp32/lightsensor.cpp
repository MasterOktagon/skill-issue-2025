#include <iterator>
#include <Arduino.h>
#include <cstdint>
#include <initializer_list>
#include <string>
#include <ArduinoJson.h>
#include <SPIFFS.h>
#include <FS.h>

#include "lightsensor.h"
#include "shiftregister.h"
#include "adc.h"

using namespace std;

void fs::setup(){
    Serial.println("mounting SPIFFS");
    if (!SPIFFS.begin(true)) {
        Serial.println("An Error has occurred while mounting SPIFFS");
    }
}


lightSensorArray::lightSensorArray(uint8_t led_pin,
            const uint8_t left_outer,
            const uint8_t left,
            const uint8_t center,
            const uint8_t right,
            const uint8_t right_outer){
            
    this->led_pin = led_pin;
    Serial.println(left_outer);
    this->left_outer.adc_pin  = ADC_PT_L_0;
    this->left.adc_pin        = left;
    this->center.adc_pin      = center;
    this->right.adc_pin       = right;
    this->right_outer.adc_pin = right_outer;
}

void lightSensorArray::led_on(){
    shiftregister::set(led_pin, HIGH);
    delayMicroseconds(70);
}

void lightSensorArray::led_off(){
    shiftregister::set(led_pin, LOW, false);
}


#define cal(side)                                \
    current_value = adc::read(side.adc_pin);     \
    if (i > 20){                                 \
        side.min = min(side.min, current_value); \
        side.max = max(side.max, current_value); \
    }

void lightSensorArray::calibrate_turn(uint16_t i){
        led_on();
        int16_t current_value;
        cal(left_outer);
        //Serial.println(current_value);
        cal(left);
        cal(center);
        cal(right);
        cal(right_outer);
        
        led_off();
}

void read_side(lsData& side){
    
    int a = ((adc::read(side.adc_pin) - side.min) * 100);
    side.value = int16_t(a / (side.max - side.min));

}

void lightSensorArray::read(){
    
    led_on();
    read_side(left_outer);
    read_side(left);
    read_side(center);
    read_side(right);
    read_side(right_outer);
    led_off();
}

string lightSensorArray::_str(){
    string s = "LightSensorArray:\t{L_1 ";
    s += to_string(left_outer.min)  + "/" + to_string(left_outer.max)  + " \tL_0 ";
    s += to_string(left.min)        + "/" + to_string(left.max)        + " \tM ";
    s += to_string(center.min)      + "/" + to_string(center.max)      + " \tR_0 ";
    s += to_string(right.min)       + "/" + to_string(right.max)       + " \tR_1 ";
    s += to_string(right_outer.min) + "/" + to_string(right_outer.max) + " \t}" ;

    return s;
}

string lightSensorArray::save(){
    JsonDocument data;
    data["left_outer"]["min"] = left_outer.min;
    data["left_outer"]["max"] = left_outer.max;

    data["left"]["min"] = left.min;
    data["left"]["max"] = left.max;

    data["center"]["min"] = center.min;
    data["center"]["max"] = center.max;

    data["right"]["min"] = right.min;
    data["right"]["max"] = right.max;

    data["right_outer"]["min"] = right_outer.min;
    data["right_outer"]["max"] = right_outer.max;

    string s;
    serializeJson(data, s);
    return s;
}

void lightSensorArray::load(String data){
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, data);

    if (error) {
        Serial.print(F("local deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
    }

    left_outer.min = doc["left_outer"]["min"];
    left_outer.max = doc["left_outer"]["max"];

    left.min = doc["left"]["min"];
    left.max = doc["left"]["max"];

    center.min = doc["center"]["min"];
    center.max = doc["center"]["max"];

    right.min = doc["right"]["min"];
    right.max = doc["right"]["max"];

    right_outer.min = doc["right_outer"]["min"];
    right_outer.max = doc["right_outer"]["max"];
}

namespace ls{
    lightSensorArray white(SR_PT_WHITE);
    lightSensorArray green(SR_PT_GREEN);
    lightSensorArray red(SR_PT_RED);
    #if (BOARD_REVISION > 1)
        lightSensorArray back(SR_PT_WHITE, ADC_PT_BACK_L_1, ADC_PT_BACK_L_0, ADC_PT_BACK_L_0, ADC_PT_BACK_R_0, ADC_PT_BACK_R_1);
        lightSensorArray* all[4] = {&white, &green, &red, &back};
    #else 
        #define back nullptr
        lightSensorArray* all[4] = {&white, &green, &red, back};
    #endif


}


const void ls::read(){
    white.read();
    red.read();
    green.read();
    #if (BOARD_REVISION > 1)
        back.read();
    #endif
}

void ls::read(initializer_list<lightSensorArray*> ls){
    for (lightSensorArray* l : ls){
        l->read();
    }
}

void ls::calibrate(uint16_t iterations, uint16_t delay_ms){
    Serial.print("Calibrating...");
    for(uint16_t i = 0; i < iterations; i++){
        for(uint8_t ls = 0; ls < 4; ls++){
            if (all[ls] != nullptr){
                all[ls]->calibrate_turn(i);
            }
        }
        delay(delay_ms);
    }
    Serial.println("Succes!");
}

void ls::save(){
    File file = SPIFFS.open("/calibration.json","w");
    if (!file){
      Serial.println("ERROR opening write file!");
      file.close();
      return;
    }
    
    JsonDocument doc;
    doc["white"] = white.save();
    doc["green"] = green.save();
    doc["red"]   = red.save();
    #if (BOARD_REVISION > 1)
        doc["back"] = back.save();
    #endif
    serializeJson(doc, file);

    file.close();
}

void ls::load(){
    Serial.println("Loading from file...");
    File f = SPIFFS.open("/calibration.json","r");

    if (!f){
        Serial.println("ERROR opening read file!");
        f.close();
        return;
    }

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, f.readString());

    if (error) {
        Serial.print(F("global deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
    }
    white.load(doc["white"]);
    green.load(doc["green"]);
    red.load(doc["red"]);
    #if (BOARD_REVISION > 1)
        red.load(doc["back"]);
    #endif

    f.close();
}




