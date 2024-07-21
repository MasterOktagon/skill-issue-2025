#include "globals.hpp"
#include "kmeans.hpp"
#include <cmath>
#include <numeric>
#include <opencv2/opencv.hpp>

#define degrees(a) a * (180.0 / 3.141592653589793238463)
vector<tuple<uint8_t, int8_t>> find_victims() {

  for (int _ = 0; _ < 1; _++) {
    // read frame, blurr and set to gray
    cv::Mat frame;
    cam->read(frame);
    cv::GaussianBlur(frame, frame, cv::Size(3, 3), 1);
    cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);
    // edge detection
    // cv::Mat edges;
    // cv::Canny(frame, edges, 60, 100, 3, false);
    // cv::dilate(edges, edges, 4);
    cv::Mat blurred, entropy;
    cv::GaussianBlur(frame, blurred, cv::Size(31, 31), 2);
    entropy = frame - blurred;
    cv::inRange(entropy, cv::Scalar(3, 3, 3), cv::Scalar(255, 255, 255),
                entropy);
    cv::rectangle(entropy, cv::Point2i(0, 0),
                  cv::Point2i(entropy.cols, int(entropy.rows * 2 / 5)),
                  cv::Scalar(0, 0, 0), cv::FILLED);

    cv::erode(entropy, entropy, 2);
    cv::dilate(entropy, entropy, 2);
    cv::erode(entropy, entropy, 2);
    cv::dilate(entropy, entropy, 2);
    cv::Mat blobs, centroids, stats;
    cv::connectedComponentsWithStats(entropy, blobs, stats, centroids, 8);

    // get sizes out of the stats matrix
    vector<cv::Scalar> sizes{};
    for (int i = 0; i < stats.rows; i++) {
      sizes.push_back(stats.at<int>(int(i), stats.cols - 1));
    }
    sizes.erase(sizes.begin()); // avoid the background

    cv::Mat edges(entropy.rows, entropy.cols, CV_8UC1, cv::Scalar(0, 0, 0));
    for (size_t i = 0; i < sizes.size(); i++) {
      if (sizes[i][0] >= 50) {
        cv::Mat m;
        cv::inRange(blobs, cv::Scalar(i + 1), cv::Scalar(i + 1), m);
        edges.setTo(cv::Scalar(255, 255, 255), m);
      }
    } // end of oscars edge detection
    // circle detection
    vector<cv::Vec3f> circles;
    cv::HoughCircles(edges, circles, cv::HOUGH_GRADIENT, 1, 10, 54, 26, 15,
                     150);
    cv::cvtColor(frame, frame, cv::COLOR_GRAY2BGR);
    // unzip clusters to centers and radii
    vector<cluster> clusters = {};
    vector<int> cluster_radiuses = {};
    if (circles.size() > 0) {
      vector<cv::Point2i> centers = {};
      vector<int> radii = {};
      for (cv::Vec3f c : circles) {
        centers.push_back(cv::Point2i(c[0], c[1]));
        radii.push_back(c[2]);
      }
      // kmeans
      clusters = kmeans(centers, 3);
      // remove clusters whose center is above the middle
      int s = clusters.size();
      int i = 0;
      for (int _ = 0; _ < s; _++) {
        cluster c = clusters[i];
        if (get<0>(c).y < frame.rows / 2) {
          clusters.erase(clusters.begin() + i);
        } else {
          i++;
        }
      }
      // backmatch the circles of the clusters to their radii
      for (cluster c : clusters) {
        vector<cv::Point2i> cpoints = get<1>(c);
        vector<int> available_radiuses = {};
        for (cv::Point2i p : cpoints) {
          auto match = find(centers.begin(), centers.end(), p);
          available_radiuses.push_back(radii[distance(centers.begin(), match)]);
        }
        // take the average of the radii
        cluster_radiuses.push_back(int(accumulate(available_radiuses.begin(),
                                                  available_radiuses.end(), 0) /
                                       available_radiuses.size()));
      }
      output << "anfang gefährlich" << endl;
      output << "ende gefährlich" << endl;
      // validate clusters
      //       for (size_t i = 0; i < clusters.size(); i++) {
      //         bool valid = 1;
      //         // check color of 4 points left and right of the center of the
      //         cluster const float diff = radii[i] * 2 / 3; const int y =
      //         get<0>(clusters[i]).y; for (int j = -2; j < 3; j++) {
      //           if (j == 0) {
      //             continue;
      //           };
      //           const int x =
      //               get<0>(clusters[i]).x + copysign(radii[i], j) + diff * j;
      //           if (!(frame.cols > x and x > 0)) {
      //             continue;
      //           };
      //           if (frame.at<cv::Scalar>(int(x), int(y))[0] > 200) {
      //             cv::circle(frame, cv::Point2i(x, y), 3, cv::Scalar(0, 0,
      //             255), -1,
      //                        8, 0);
      //             valid = 0;
      //             break;
      //           }
      // #ifdef DEBUG
      //           output << get<0>(clusters[i]).x << " " <<
      //           get<0>(clusters[i]).y
      //                  << "\n";
      // #endif
      //         }
      //         if (!valid) {
      // #ifdef DEBUG
      //           output << get<0>(clusters[i]).x << " " <<
      //           get<0>(clusters[i]).y
      //                  << "    is not valid" << "\n";
      // #endif
      //         };
      //       }
      // drawing
      for (cv::Vec3f c : circles) {
        cv::Point2i p = cv::Point2i(int(c[0]), int(c[1]));
        // draw the circle center
        cv::circle(frame, p, 3, cv::Scalar(0, 0, 0), -1, 8, 0);
        // draw the circle outline
        cv::circle(frame, p, int(c[2]), cv::Scalar(0, 0, 0), 2, 8, 0);
      }

      i = 0;
      for (cluster c : clusters) {
        cv::Point2i p = get<0>(c);
        // draw the cluster center
        cv::circle(frame, p, 3, cv::Scalar(255, 0, 0), -1, 8, 0);
        // draw the cluster outline
        cv::circle(frame, p, cluster_radiuses[i++], cv::Scalar(255, 0, 0), 2, 8,
                   0);
      }
    }
    // draw horizon
    frame.setTo(cv::Scalar(0, 255, 0), edges);
    cv::line(frame, cv::Point2i(0, int(frame.rows / 2)),
             cv::Point2i(frame.cols, int(frame.rows / 2)),
             cv::Scalar(0, 0, 255), 2);
    cv::line(frame, cv::Point2i(int(frame.cols / 2), 0),
             cv::Point2i(int(frame.cols / 2), frame.rows),
             cv::Scalar(0, 0, 255), 2);
    // draw line to center of first circle
    vector<tuple<uint8_t, int8_t>> victims = {};
    if (clusters.size() > 0) {
      for (uint i = 0; i < clusters.size(); i++) {
        float correction;
        cv::line(frame, cv::Point2i(int(frame.cols / 2), frame.rows),
                 cv::Point2i(get<0>(clusters[i]).x, int(frame.rows / 2)),
                 cv::Scalar(0, 0, 255), 2);
        correction = degrees(asin((get<0>(clusters[i]).x - frame.cols / 2) /
                                  (frame.rows / 2))) /
                     90 * (CAM_FOV / 2);
        // draw correction value
        cv::putText(frame, to_string(correction) + "d",
                    cv::Point2i(10, frame.rows), cv::FONT_HERSHEY_SIMPLEX, 1,
                    cv::Scalar(255, 0, 0));
        victims.push_back(
            tuple<uint8_t, int8_t>(1000 / cluster_radiuses[i], correction));
      }
    }
    show_frame(frame);
    return victims;
  }
  return {};
}
// */
