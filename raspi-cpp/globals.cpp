
#include <iostream>
#include <fstream>
#include <cstdint>
#include <chrono>
#include <thread>

// OpenCV
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/opencv.hpp>

#include "globals.hpp"

using namespace std;
using namespace std::this_thread;     // sleep_for, sleep_until
using namespace std::chrono_literals; // ns, us, ms, s, h, etc.
using std::chrono::system_clock;


cv::VideoCapture* cam;
cv::VideoWriter vout;
cv::Size frame_size;
ostream output (NULL); // WTF I need NULL?


void init(){
    // use log files to track output to acces it later
    #ifndef DEBUG
        ofstream of; of.open("out.log"); // Assign a file as target
        ostream output(of.rdbuf());
    #else
        ostream output(cout.rdbuf()); // Assign cout as target for debug output
    #endif

    output << "Program started!\n";

    uint8_t cam_idx;
    
    // TODO: load camera index from file
    #ifdef DEBUG
        cin >> cam_idx;
    #endif

    sleep_for(4s);

    cam = new cv::VideoCapture(cam_idx);
    frame_size = cv::Size(20,20);//cv::Size(cam.get(cv::CAP_PROP_FRAME_WIDTH), cam.get(cv::CAP_PROP_FRAME_HEIGHT));
    vout = cv::VideoWriter("run.avi", 0, 5, frame_size);

    output << "Init complete\n";
}


