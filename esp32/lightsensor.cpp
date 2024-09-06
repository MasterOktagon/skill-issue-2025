
#include <Arduino.h>
#include <cstdint>
#include <initializer_list>
#include <string>
#include <ArduinoJson.h>
#include <SPIFFS.h>

#include "lightsensor.h"
#include "shiftregister.h"


#define LED_DELAY 80
#define ITER_SKIP 20

using namespace std;

void fs::setup(){
    Serial.println("mounting SPIFFS");
    if (!SPIFFS.begin(true)) {
        Serial.println("An Error has occurred while mounting SPIFFS");
    }
}

lightSensor::lightSensor(uint8_t led_pin, uint8_t sensor_pin){
    this->led_pin = led_pin;
    this->sensor_pin = sensor_pin;
}

lightSensor::lightSensor(){
    led_pin = 0;
    sensor_pin = 0;
}

void lightSensor::led_on(){
    digitalWrite(led_pin, HIGH);
    delayMicroseconds(LED_DELAY);
}

void lightSensor::led_off(){
    digitalWrite(led_pin, LOW);
}

void lightSensor::read(){
    // read value and map it to a range between 0 and 100
    int a = ((analogRead(sensor_pin) - vmin) * 100);
    value = int16_t(a / (vmax - vmin));
}

void lightSensor::calibrate_turn(int iter){
    led_on();

    int16_t val = analogRead(sensor_pin); // read light sensor value
    if (iter > ITER_SKIP){
        // update min/max values
        vmax = max(vmax, val);
        vmin = min(vmin, val);
    }

    led_off();
}

int16_t lightSensor::get_max(){return vmax;}
int16_t lightSensor::get_min(){return vmin;}

RGBSensor::RGBSensor(uint8_t sensor_pin, uint8_t led_idx, Adafruit_NeoPixel* leds, uint32_t color){
    this->led_idx = led_idx;
    this->led = leds;
    this->color = color;
    this->sensor_pin = sensor_pin;
}

void RGBSensor::led_on(){
    led->setPixelColor(led_idx, color);
    led->show();
}

void RGBSensor::led_off(){
    led->setPixelColor(led_idx, led->Color(0,0,0)); // black = Off
    led->show();
}

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
    string s = "LightSensorArray:\t{L_1 ";
    s += to_string(left_outer.get_min())  + "/" + to_string(left_outer.get_max())  + " \tL_0 ";
    s += to_string(left.get_min())        + "/" + to_string(left.get_max())        + " \tR_0 ";
    s += to_string(right.get_min())       + "/" + to_string(right.get_max())       + " \tR_1 ";
    s += to_string(right_outer.get_min()) + "/" + to_string(right_outer.get_max()) + " \t}" ;

    return s;
}

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
    left_outer.read();
    left.read();
    right.read();
    right_outer.read();
}

void lightSensorArray::load(String data){
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, data);

    if (error) {
        Serial.print(F("local deserializeJson() failed: "));
        Serial.println(error.f_str());
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

    Adafruit_NeoPixel led (6, PT_RGB);

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
        RGBSensor(PT_L_1, 3, &led, led.Color(0, 255, 0)),
        RGBSensor(PT_L_0, 3, &led, led.Color(0, 255, 0)),
        RGBSensor(PT_R_0, 2, &led, led.Color(0, 255, 0)),
        RGBSensor(PT_R_1, 2, &led, led.Color(0, 255, 0))
    );
    lightSensorArray green_b(
        RGBSensor(PT_L_3, 5, &led, led.Color(0, 255, 0)),
        RGBSensor(PT_L_2, 5, &led, led.Color(0, 255, 0)),
        RGBSensor(PT_R_2, 4, &led, led.Color(0, 255, 0)),
        RGBSensor(PT_R_3, 4, &led, led.Color(0, 255, 0))
    );

    lightSensorArray red(
        RGBSensor(PT_L_1, 3, &led, led.Color(255, 0, 0)),
        RGBSensor(PT_L_0, 3, &led, led.Color(255, 0, 0)),
        RGBSensor(PT_R_0, 2, &led, led.Color(255, 0, 0)),
        RGBSensor(PT_R_1, 2, &led, led.Color(255, 0, 0))
    );
    lightSensorArray red_b(
        RGBSensor(PT_L_3, 5, &led, led.Color(255, 0, 0)),
        RGBSensor(PT_L_2, 5, &led, led.Color(255, 0, 0)),
        RGBSensor(PT_R_2, 4, &led, led.Color(255, 0, 0)),
        RGBSensor(PT_R_3, 4, &led, led.Color(255, 0, 0))
    );

    lightSensorArray* all[6] = {&white, &green, &red, &white_b, &green_b, &red_b};
}


const void ls::read(){
    white.read();
    red.read();
    green.read();

    white_b.read();
    red_b.read();
    green_b.read();
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

    doc["white_b"] = white_b.save();
    doc["green_b"] = green_b.save();
    doc["red_b"]   = red_b.save();

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

    white_b.load(doc["white_b"]);
    green_b.load(doc["green_b"]);
    red_b.load(doc["red_b"]);

    f.close();
}

void ls::setup(){
    led.begin();
    led.clear();
    led.show();

    
    pinMode(WHITE_L, OUTPUT);
    pinMode(WHITE, OUTPUT);
    pinMode(WHITE_R, OUTPUT);

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


