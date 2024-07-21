#pragma once
#include <cstdint>
#include <tuple>
#include <vector>
/*
File that handles all the communication with the ESP32
*/
using namespace std;
namespace i2c {

enum Task {
  /*
  Represents different tasks/states given by the ESP32
  */
  WAIT,
  FIND_VICTIM,
  FIND_CORNER,
  FIND_EXIT,
  END
};

extern Task get_task();
/*
[return] get the task given by the ESP32
*/

extern void send_victims(vector<tuple<uint8_t, int8_t>> data);
/*
Send victim data back
*/
} // namespace i2c
