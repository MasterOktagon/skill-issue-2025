#include <chrono>
#include <cstdint>
#include <string>
#include <thread>
#include <vector>
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
  // release cam and destroy all windows
  cam->release();
  cv::destroyAllWindows();
  delete cam;
#ifndef DEBUG
  output.close();
#endif
}

int main(int argc, char **argv) {
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
  } catch (...) {
    output << "Startup ERRROR" << endl;
    cleanup();
    return _PROGRAM_SETUP_ERROR;
  }

  try {
    loop {
      cv::Mat frame;
      cam->read(frame);
      int task;
      switch (task = i2c_get_task()) {
      case WAIT:
        output << "WAIT" << endl;
        sleep_for(100ms);
        break;
      case FIND_VICTIM:

        break;
      default:
        output << "HELOLLOLOLOLL" << endl;
        break;
      }
      // output << "Task: " << task << endl;

      show_frame(frame);
#ifdef DEBUG
      // close video when user presses any key
      char pressed_char;
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
  cleanup();
  return _PROGRAM_END;
}
