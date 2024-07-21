
/*
Main Program rev 3

copyright (c) SkillIssue, 2025
licensed under CC-BY-NC-4.0
*/

#include <chrono>
#include <cstdint>
#include <json-c/json.h>
#include <mutex>
#include <string>
#include <thread>
#include <tuple>
#include <vector>

using namespace std;

// OpenCV
#include <opencv2/core.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include "globals.hpp"

bool in_room = false;

int main() {
  // Initialize Systems
  init();

  // TODO wait for button press

  cv::VideoCapture cam = cv::VideoCapture(2);
  loop {
    cv::Mat frame;
    cam.read(frame); // get newest frame
    cv::resize(frame, frame, cv::Size(160, 120));
    // cout << cam.get(cv::CAP_PROP_FRAME_WIDTH) << "\n";
    // cout << cam.get(cv::CAP_PROP_FRAME_HEIGHT) << "\n";
    cv::Mat test[3] = {};
    cv::split(frame, test);
    cv::Mat schrott = 2 * test[2] - test[1] - test[0];
    // cv::Mat schrott = test[2] - 0.5 * test[1] - 0.5 * test[0];
    // cv::Mat schrott2 = 2 * test[1] - test[2] - test[0];
    // edge
    // cv::cvtColor(schrott, schrott, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(schrott, schrott, cv::Size(3, 3), 2);
    cv::Mat schrotttie;
    cv::Canny(schrott, schrotttie, 100, 150, 3, false);
    vector<cv::Vec2f> lines;
    cv::dilate(schrotttie, schrotttie, 2);
    cv::erode(schrotttie, schrotttie, 2);
    cv::HoughLines(schrotttie, lines, 1, CV_PI / 180, 40, 0, 0);
    cv::Mat hallo;
    // kanten detection
    cv::cvtColor(schrotttie, schrotttie, cv::COLOR_GRAY2BGR);
    cv::cvtColor(schrotttie, hallo, cv::COLOR_BGR2GRAY);
    cv::cvtColor(hallo, hallo, cv::COLOR_GRAY2BGR);
    for (auto elem : lines) {
      // TODO: WTF
      float rho = elem[0];
      float theta = elem[1];
      cv::Point pt1, pt2;
      double a = cos(theta), b = sin(theta);
      double x0 = a * rho, y0 = b * rho;
      pt1.x = cvRound(x0 + 1000 * (-b));
      pt1.y = cvRound(y0 + 1000 * (a));
      pt2.x = cvRound(x0 - 1000 * (-b));
      pt2.y = cvRound(y0 - 1000 * (a));
      cv::line(hallo, pt1, pt2, cv::Scalar(0, 0, 255), 1, cv::LINE_AA);
      // cv::circle(hallo, pt1, 3, cv::Scalar(0, 255, 0), 2, cv::LINE_8, 2);
      // cv::circle(hallo, pt2, 3, cv::Scalar(0, 255, 0), 2, cv::LINE_8, 2);
      cout << pt1 << "\n";
      cout << pt2 << "\n";
      cv::Point p1, p2;
      p1.x = cvRound(x0 + 1 * (-b));
      p1.y = cvRound(y0 + 1 * (a));
      p2.x = cvRound(x0 - 1 * (-b));
      p2.y = cvRound(y0 - 1 * (a));
      cv::rectangle(hallo, p1, p2, cv::Scalar(0, 255, 0));
    }

#ifdef DEBUG
    cv::imshow("frame", frame);
    cv::imshow("test0", test[0]);
    cv::imshow("test1", test[1]);
    cv::imshow("test2", test[2]);
    cv::imshow("schrott", schrott);
    // cv::imshow("schrott2", schrott2);
    // cv::imshow("schrotttie", schrotttie);
    cv::imshow("LINES", hallo);
#endif

    char c;
    if ((c = cv::waitKey(5)) > 0) {
      break;
    }
  }

  cam.release();
  cv::destroyAllWindows();

#ifndef DEBUG
  // of.close();
#endif
  return _PROGRAM_END;
}
