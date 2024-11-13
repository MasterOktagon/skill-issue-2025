#include "VL53L1X.h"
#include "shiftregister.h"
#include "Pins.h"

#include "tof.h"


namespace tof {
    VL53L1X front, left, right, back;
}


void tof::setup(){
  VL53L1X front = VL53L1X();
  shiftregister::set(SR_XSHT_1, HIGH);
  delay(10);
  if (!front.init()) {
    output.println("MISSING TOF 1 FRONT!!!");
  }
  else {
    front.setAddress(TOF_ADDRESS1);
    front.setTimeout(500);
    front.setDistanceMode(VL53L1X::Short);
  }
  VL53L1X left = VL53L1X();
  shiftregister::set(SR_XSHT_2, HIGH);
  delay(10);
  if (!left.init()) {
    output.println("MISSING TOF 2 LEFT!!!");
  }
  else {
    left.setAddress(TOF_ADDRESS2);
    left.setTimeout(500);
    left.setDistanceMode(VL53L1X::Short);
  }
  VL53L1X right = VL53L1X();
  shiftregister::set(SR_XSHT_4, HIGH);
  delay(10);
  if (!right.init()) {
    output.println("MISSING TOF 4 RIGHT!!!");
  }
  else {
    right.setAddress(TOF_ADDRESS4);
    right.setTimeout(500);
    right.setDistanceMode(VL53L1X::Short);
  }
  VL53L1X claw = VL53L1X();
  shiftregister::set(SR_XSHT_3, HIGH);
  delay(10);
  if (!claw.init()) {
    output.println("MISSING TOF 3 CLAW!!!");
  }
  else {
    claw.setAddress(TOF_ADDRESS3);
    claw.setTimeout(500);
    claw.setDistanceMode(VL53L1X::Short);
  }

  output.println("tofs active");
}

void tof::start_all(){

}

void tof::stop_all(){

}

