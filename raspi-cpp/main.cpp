
/*
Main Program rev 3

copyright (c) SkillIssue, 2025
licensed under CC-BY-NC-4.0
*/

#include <mutex>
#include <thread>
#include <vector>
#include <string>
#include <chrono>
#include <json-c/json.h>
#include <cstdint>

using namespace std;

// OpenCV
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/opencv.hpp>

#include "globals.hpp"

bool in_room = false;


int main(){
    //Initialize Systems
    init();

    // TODO wait for button press

    cv::VideoCapture cam = cv::VideoCapture(0);
    loop {
        cv::Mat frame;
        cam.read(frame); // get newest frame

        #ifdef DEBUG
            cv::imshow("frame",frame);
        #endif

        char c;
        if ((c = cv::waitKey(5)) > 0){
            break;
        }
    }

    

    cam.release();
    cv::destroyAllWindows();

    #ifndef DEBUG
        of.close();
    #endif
    return _PROGRAM_END;
}





