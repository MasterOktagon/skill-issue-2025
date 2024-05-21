#include <opencv2/core/types.hpp>
#include <vector>
#include <tuple>
#include <cstdint>
#include <cstdlib>


#include "kmeans.hpp"

using namespace std;

struct IndexError{};

cv::Point2i get_random_point(vector<cv::Point2i> p){
    uint16_t sel = rand() % p.size();
    return p[sel];
}


vector<cluster> kmeans(vector<cv::Point2i> points, uint8_t k){
    // create random cluster point
    vector<cv::Point2f> clusterpoints = {get_random_point(points)};

    return {};
}

