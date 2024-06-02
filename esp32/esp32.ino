
#include "motor.h"
#include "shiftregister.h"
#include "lightsensor.h"
#include "adc.h"
#include "linefollower.h"

void setup(){
    Serial.begin(115200);
    Serial.println("Serial init");
    shiftregister::setup();
    shiftregister::reset();
    ls::calibrate(5000);

    // Print min/max values
    Serial.print("white "); Serial.println(ls::white._str().c_str());
    Serial.print("green "); Serial.println(ls::green._str().c_str());
    Serial.print("red "); Serial.println(ls::red._str().c_str());

}

void loop(){
    ls::read();
    lf::follow();
    //delay(100);

}


