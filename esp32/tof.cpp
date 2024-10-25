#include "VL53L1X.h"
#include "shiftregister.h"
#include "Pins.h"

#include "tof.h"


namespace tof {
    VL53L1X front, left, right, back;
}


void tof::setup(){
  auto front = VL53L1X();
  shiftregister::set(SR_XSHT_1, HIGH);
  if (!front.init()) {
    output.println("MISSING TOF FRONT!!!");
  }
  front.setAddress(TOF_ADDRESS1);
  auto left = VL53L1X();
  shiftregister::set(SR_XSHT_2, HIGH);
  if (!left.init()) {
    output.println("MISSING TOF LEFT!!!");
  }
  left.setAddress(TOF_ADDRESS2);
  auto right = VL53L1X();
  shiftregister::set(SR_XSHT_3, HIGH);
  if (!right.init()) {
    output.println("MISSING TOF RIGHT!!!");
  }
  right.setAddress(TOF_ADDRESS3);
  auto claw = VL53L1X();
  shiftregister::set(SR_XSHT_4, HIGH);
  if (!claw.init()) {
    output.println("MISSING TOF CLAW!!!");
  }
  claw.setAddress(TOF_ADDRESS4);
  front.setDistanceMode(VL53L1X::Short);
  left.setDistanceMode(VL53L1X::Short);
  right.setDistanceMode(VL53L1X::Short);
  claw.setDistanceMode(VL53L1X::Short);
  


}

void tof::start_all(){

}

void tof::stop_all(){

}

