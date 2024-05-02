
#define BOARD_REVISION 1

#include "shiftregister.h"

void setup(){
    Serial.begin(115200);
    Serial.println("Serial init");
    shiftregister::setup();
    shiftregister::reset();
    

}

void loop(){
  shiftregister::set(SR_LED_L_GREEN, !shiftregister::get(SR_LED_L_GREEN));
  delay(1000);

}


