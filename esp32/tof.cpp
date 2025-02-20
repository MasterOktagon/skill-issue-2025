#include "VL53L1X.h"
#include "shiftregister.h"
#include "Pins.h"

#include "tof.h"


namespace tof {
    VL53L1X front, left, right, back;
}


void tof::setup(){
  VL53L1X front = VL53L1X();
  shiftregister::set(SR_XSHT_2, HIGH);
  delay(10);
  if (!front.init()) {
    output.println("ERROR: Missing TOF 1 front!!!");
  }
  else {
    front.setAddress(TOF_ADDRESS1);
    front.setTimeout(500);
    front.setDistanceMode(VL53L1X::Short);
  }
  VL53L1X left = VL53L1X();
  shiftregister::set(SR_XSHT_2, LOW, false);
  shiftregister::set(SR_XSHT_1, HIGH);
  delay(10);
  if (!left.init()) {
    output.println("ERROR: Missing TOF 2 left!!!");
  }
  else {
    left.setAddress(TOF_ADDRESS2);
    left.setTimeout(500);
    left.setDistanceMode(VL53L1X::Short);
  }
  VL53L1X right = VL53L1X();
  shiftregister::set(SR_XSHT_1, LOW, false);
  shiftregister::set(SR_XSHT_4, HIGH);
  delay(10);
  if (!right.init()) {
    output.println("ERROR: Missing TOF 4 right!!!");
  }
  else {
    right.setAddress(TOF_ADDRESS4);
    right.setTimeout(500);
    right.setDistanceMode(VL53L1X::Short);
  }
  VL53L1X claw = VL53L1X();
  shiftregister::set(SR_XSHT_4, LOW, false);
  shiftregister::set(SR_XSHT_3, HIGH);
  delay(10);
  if (!claw.init()) {
    output.println("ERROR: Missing TOF 3 claw!!!");
  }
  else {
    claw.setAddress(TOF_ADDRESS3);
    claw.setTimeout(500);
    claw.setDistanceMode(VL53L1X::Short);
  }
  shiftregister::set(SR_XSHT_3, LOW);

  output.println("INFO: tofs active");
}

void tof::start_all(){
    
}

void tof::stop_all(){

}

