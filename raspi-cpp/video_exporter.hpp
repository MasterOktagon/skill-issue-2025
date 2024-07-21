#pragma once

#include <opencv2/core.hpp> // Basic OpenCV structures (cv::Mat)
#include <opencv2/videoio.hpp> // Video write

const string video_file_name = "video.avi";
VideoWriter writer;

extern void start_export();
/*
Start and setup exporting
*/

extern void export_frame();
/*
Export a key frame
*/

extern void stop_export();
/*
Stop export
*/





