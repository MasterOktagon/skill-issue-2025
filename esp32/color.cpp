
#include "color.h"
#include "lightsensor.h"

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
    counter_l = max(0,min(CHECK_LEN + 3, counter_l + get_as_multiplier(func(w,g,r, Side::LEFT))));
    counter_r = max(0,min(CHECK_LEN + 3, counter_l + get_as_multiplier(func(w,g,r, Side::RIGHT))));
}

Side color::color::operator () (){
    return Side((counter_l >= CHECK_LEN)*Side::LEFT + (counter_r >= CHECK_LEN)*Side::RIGHT);
}

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
        default:
            return g->right.value - r->right.value >= GREEN_THRESHOLD;
    }
}

bool black_detection(lightSensorArray* w, lightSensorArray* g, lightSensorArray* r, Side s){
    switch (s){
        case Side::LEFT:
            return w->left_outer.value <= BLACK_THRESHOLD;
        default:
            return w->right_outer.value <= BLACK_THRESHOLD;
    }
}

color::color color::red = color(red_detection);
color::color color::green = color(green_detection);
color::color color::black = color(black_detection);

void color::update(){
    red.update(&ls::white, &ls::green, &ls::red);
    green.update(&ls::white, &ls::green, &ls::red);
    black.update(&ls::white, &ls::green, &ls::red);
}

void color::update(initializer_list<color*> colors){
    for(color* c : colors){
        c->update(&ls::white, &ls::green, &ls::red);
    }
}
