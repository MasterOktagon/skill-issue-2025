#include "i2c.hpp"
#include "globals.hpp"
#include <cstdint>
#include <vector>
using namespace std;

i2c::Task i2c::get_task() {
  // TODO:
  // return WAIT;
  return FIND_VICTIM;
}

void i2c::send_victims(vector<tuple<uint8_t, int8_t>> victims) {
  // TODO:
  output << victims.size() << "\n";
}
