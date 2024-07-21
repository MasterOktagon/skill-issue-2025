
/*
This file contains all variables that are shared between multiple files
*/

#include <fstream>
#include <iostream>

using namespace std;

// OpenCV
#include <opencv2/core.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

// FLAGS
#define DEBUG

// constants
#define CAM_FOV 30

// MACROS
#define _PROGRAM_END 0
#define _PROGRAM_SETUP_ERROR 1
#define loop while (true)

extern cv::VideoWriter vout;
extern cv::VideoCapture *cam;
extern ostream output;

extern void init(int camdix);
/*
Init all global subsystems/variables

[param camidx] camera index to use
*/

extern void show_frame(cv::Mat & frame);
/*
Show or debug a Frame or put it into the video
*/
