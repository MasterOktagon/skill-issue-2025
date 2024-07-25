#pragma once

/*
provides a k-means distribution with special starting points
*/

#include <opencv2/core/types.hpp>
#include <vector>
#include <tuple>
#include <cstdint>

using namespace std;

// a cluster type usek in k_means.cpp
// tuple[0] is the cluster point
// tuple[1] is the vector of assigned Points
using cluster = tuple<cv::Point2f, vector<cv::Point2i>>;

extern vector<cluster> kmeans(vector<cv::Point2i>, uint8_t k);
/*
runs a k_means distribution

[param k] the amount of clusters to be generated at maximum. Bigger than 0.
[return] all found clusters (at least 1)
*/
