
/*
This file contains all variables that are shared between multiple files
*/

#include <iostream>
#include <fstream>

using namespace std;


// OpenCV
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/opencv.hpp>

// FLAGS
#define DEBUG

// constants
#define CAM_FOV 30

// MACROS
#define _PROGRAM_END 0
#define loop while(true)

extern cv::VideoWriter vout;
extern void init();
extern ostream output;


