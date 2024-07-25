#pragma once

#include <cstdint>
#include <tuple>
#include <vector>

using namespace std;


extern vector<tuple<uint8_t, int8_t>> find_victims();
/*
find victims using the camera frame

[return] vector of all found victims as a (distance, angular offset) tuple. distance is given in an arbitrary unit
         and might need testing
*/



