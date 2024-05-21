
#include <opencv2/imgproc/imgproc.hpp>
#include <cstdint>
#include <vector>

#include "kmeans.hpp"

using namespace std;


int main(){

    kmeans(vector<cv::Point2i>({cv::Point2i(0,0)}), uint8_t(3));
    return 0;
}

