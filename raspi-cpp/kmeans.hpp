#pragma once

#include <opencv2/core/types.hpp>
#include <vector>
#include <tuple>
#include <cstdint>

using namespace std;


#define cluster tuple<cv::Point2f, vector<cv::Point2f>>

extern vector<cluster> kmeans(vector<cv::Point2i>, uint8_t k);

