
//
// COLOR.cpp
//
// implements the color detection
//

#include "color.h"
#include "lightsensor.h"
#include "shared.h"

color::color::color(fsignal<bool, lightSensorArray*, lightSensorArray*, lightSensorArray*, Side> method){
    func = method;
    counter_l, counter_r = 0;
}

inline int get_as_multiplier(bool b){
    if (b){
        return 1;
    }
    return -1;
}

void color::color::update(lightSensorArray* w, lightSensorArray* g, lightSensorArray* r){
    // The min/max configuration limits the counter in a certain frame
    counter_l = min(CHECK_LEN + 3, max(0, counter_l + get_as_multiplier(func(w,g,r, Side::LEFT))));
    counter_r = min(CHECK_LEN + 3, max(0, counter_r + get_as_multiplier(func(w,g,r, Side::RIGHT))));
}

Side color::color::operator () (){
    return Side((counter_l >= CHECK_LEN)*Side::LEFT + (counter_r >= CHECK_LEN)*Side::RIGHT);
}

Side color::color::get(){
    return Side((counter_l >= CHECK_LEN)*Side::LEFT + (counter_r >= CHECK_LEN)*Side::RIGHT);
}

void color::color::reset(){
    counter_l, counter_r = 0;
}

// The main detection functions
//
// Red/Green is detected using the difference between green and red, since a 
// green light will make red shine black (and vice versa)

bool red_detection(lightSensorArray* w, lightSensorArray* g, lightSensorArray* r, Side s){
    switch (s){
        case Side::LEFT:
            return r->left.value - g->left.value >= RED_THRESHOLD;
        default:
            return r->right.value - g->right.value >= RED_THRESHOLD;
    }
}

bool green_detection(lightSensorArray* w, lightSensorArray* g, lightSensorArray* r, Side s){
    switch (s){
        case Side::LEFT:
            return g->left.value - r->left.value >= GREEN_THRESHOLD;
            //return g->left.raw - r->left.raw >= GREEN_THRESHOLD;
        default:
            //return g->right.raw - r->right.raw >= GREEN_THRESHOLD-2;
            return g->right.value - r->right.value + 4 >= GREEN_THRESHOLD;
    }
}

bool green_detection_outer(lightSensorArray* w, lightSensorArray* g, lightSensorArray* r, Side s){
    switch (s){
        case Side::LEFT:
            return g->left_outer.value - r->left_outer.value >= GREEN_THRESHOLD;
            //return g->left.raw - r->left.raw >= GREEN_THRESHOLD;
        default:
            //return g->right.raw - r->right.raw >= GREEN_THRESHOLD-2;
            return g->right_outer.value - r->right_outer.value >= GREEN_THRESHOLD;
    }
}

// black detection is simply just checking if the light values of 'white' are lower than a threshold

bool black_detection(lightSensorArray* w, lightSensorArray* g, lightSensorArray* r, Side s){
    switch (s){
        case Side::LEFT:
            return w->left.value <= BLACK_THRESHOLD;
        default:
            return w->right.value <= BLACK_THRESHOLD;
    }
}

bool black_detection_outer(lightSensorArray* w, lightSensorArray* g, lightSensorArray* r, Side s){
    switch (s){
        case Side::LEFT:
            return w->left_outer.value <= BLACK_THRESHOLD;
        default:
            return w->right_outer.value <= BLACK_THRESHOLD;
    }
}

bool silver_detection(lightSensorArray* w, lightSensorArray* g, lightSensorArray* r, Side s){
    int16_t value;
    switch(s){
        case Side::LEFT:
            digitalWrite(PT_WHITE_REF, HIGH);
            delayMicroseconds(80);
            value = analogRead(PT_REF_L);
            digitalWrite(PT_WHITE_REF, LOW);
            delayMicroseconds(80);
            return value >= SILVER_THRESHOLD;
        default:
            digitalWrite(PT_WHITE_REF, HIGH);
            delayMicroseconds(80);
            value = analogRead(PT_REF_R);
            digitalWrite(PT_WHITE_REF, LOW);
            delayMicroseconds(80);
            return value >= SILVER_THRESHOLD;
    }
}


color::color color::red = color(red_detection);
color::color color::green = color(green_detection);
color::color color::black = color(black_detection);
color::color color::black_outer = color(black_detection_outer);
color::color color::silver = color(silver_detection);
color::color color::green_outer = color(green_detection_outer);

void color::update(){
    red.update(&ls::white, &ls::green, &ls::red);
    green.update(&ls::white, &ls::green, &ls::red);
    black.update(&ls::white, &ls::green, &ls::red);
    black_outer.update(&ls::white, &ls::green, &ls::red);
    silver.update(&ls::white, &ls::green, &ls::red);
    green_outer.update(&ls::white, &ls::green, &ls::red);
}

void color::update(initializer_list<color*> colors){
    for(color* c : colors){
        c->update(&ls::white, &ls::green, &ls::red);
    }
}
