#include <chrono>
#include <cstdint>
#include <string>
#include <thread>
#include <vector>
#include <tuple>
using namespace std;
using namespace std::this_thread;     // sleep_for, sleep_until
using namespace std::chrono_literals; // ns, us, ms, s, h, etc.

#include <opencv2/core.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include "globals.hpp"
#include "i2c.hpp"

void cleanup() {
/*
release cam and destroy all windows
*/ 
  cam->release();
  cv::destroyAllWindows();
  delete cam;

  output << "cleanup succesful!\n";
#ifndef DEBUG
  output.close(); // if output is a file, close it
#endif
}

int main(int argc, char **argv) {
/*
Main loop

[param argc] number of arguments
[param argv] values of arguments
*/  

  // cam and output init
  try {
    int camdix = 0;
    if (argc > 1) {
      // camdix from cl argument
      camdix = stoi(string(argv[1]));
    }
    init(camdix);

  } catch (exception &e) {
    output << "Startup ERRROR" << e.what() << endl;
    cleanup();
    return _PROGRAM_SETUP_ERROR;

  } catch (...) { // catch all other errors
    output << "Startup ERRROR" << endl;
    cleanup();
    return _PROGRAM_SETUP_ERROR;
  }

  try {
    loop {
      int task;
      vector<tuple<uint8_t,int8_t>> victims = {};
#ifdef DEBUG
      char pressed_char;
#endif
      switch (task = i2c::get_task()) {
      case i2c::WAIT:
        output << "WAIT\n";
        sleep_for(100ms);
        break;

      case i2c::FIND_VICTIM:
         // get viewed victims
        break;

      case i2c::END:
        output << "received END task" << endl;
        goto endpoint; // break out of loop
      
      default:
        break;
      }

      cv::Mat frame;
      cam->read(frame);
      show_frame(frame);
#ifdef DEBUG
      // abort when user presses any key
      
      if ((pressed_char = cv::waitKey(1)) > 0) {
        break;
      }
#endif
    }
  } catch (exception &e) {
    output << "Loop ERRROR" << e.what() << endl;
  } catch (...) {
    output << "Loop ERRROR" << endl;
  }
  endpoint: cleanup();
  return _PROGRAM_END;
}
