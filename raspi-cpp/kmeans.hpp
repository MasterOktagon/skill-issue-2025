#pragma once

#include <opencv2/core/types.hpp>
#include <vector>
#include <tuple>
#include <cstdint>

using namespace std;

/*
provides a k-means distribution with special starting points
*/

using cluster = tuple<cv::Point2f, vector<cv::Point2i>>;

extern vector<cluster> kmeans(vector<cv::Point2i>, uint8_t k);

