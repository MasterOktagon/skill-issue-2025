#include "VL53L1X.h"
#include "shiftregister.h"
#include "Pins.h"

#include "tof.h"


namespace tof {
    VL53L1X front, left, right, claw;
}


void tof::setup(){
  front = VL53L1X();
  enable(tof::FRONT);
  delay(10);
  if (!front.init()) {
    output.println("ERROR: Missing TOF 2 front!!!");
  }
  else {
    front.setTimeout(500);
    front.setDistanceMode(VL53L1X::Short);
  }
  left = VL53L1X();
  enable(tof::LEFT);
  delay(10);
  if (!left.init()) {
    output.println("ERROR: Missing TOF 1 left!!!");
  }
  else {
    left.setTimeout(500);
    left.setDistanceMode(VL53L1X::Short);
  }
  VL53L1X right = VL53L1X();
  enable(tof::RIGHT);
  delay(10);
  if (!right.init()) {
    output.println("ERROR: Missing TOF 4 right!!!");
  }
  else {
    right.setTimeout(500);
    right.setDistanceMode(VL53L1X::Short);
  }
  claw = VL53L1X();
  enable(tof::CLAW);
  delay(10);
  if (!claw.init()) {
    output.println("ERROR: Missing TOF 3 claw!!!");
  }
  else {
    claw.setTimeout(500);
    claw.setDistanceMode(VL53L1X::Short);
  }
  enable(tof::OFF);

  output.println("INFO: tofs active");
}

void tof::start_all(){
    
}

void tof::stop_all(){

}

void tof::enable(tof t){
    shiftregister::set(SR_XSHT_2, t == tof::FRONT, false);
    shiftregister::set(SR_XSHT_1, t == tof::LEFT, false);
    shiftregister::set(SR_XSHT_3, t == tof::CLAW, false);
    shiftregister::set(SR_XSHT_4, t == tof::RIGHT);
}

