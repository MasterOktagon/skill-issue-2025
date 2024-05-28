
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/opencv.hpp>
#include <cstdint>
#include <vector>
#include <iostream>
#include <filesystem>
#include <string>
#include <tuple>
#include <cmath>

#include "kmeans.hpp"

#define degrees(a) a*(180.0/3.141592653589793238463)
#define FOV 30

using namespace std;


int main(){
    cout << "Hello World!" << endl;
    
    int camidx;
    cout << "input camera index: ";
    cin >> camidx;
    auto cap = cv::VideoCapture(camidx);
    //for (const auto& dirEntry : filesystem::recursive_directory_iterator("./raspi-cpp/testimages")){
    while(true){
        float correction = 0;
        //cv::Mat frame = cv::imread(dirEntry.path());
        cv::Mat frame;
        cap.read(frame);
        cv:GaussianBlur(frame,frame, cv::Size(3,3), 1);
        cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);
        
        vector<cv::Vec3f> circles;
        cv::HoughCircles(frame, circles, cv::HOUGH_GRADIENT, 1, 10, 80, 40, 15, 100);
        cv::cvtColor(frame, frame, cv::COLOR_GRAY2BGR);
        vector<cluster> clusters = {};
        
        if (circles.size() > 0){
            vector<cv::Point2i> centers = {};
            vector<int> radiuses = {};
            for(cv::Vec3f c : circles){
                centers.push_back(cv::Point2i(c[0],c[1]));
                radiuses.push_back(c[2]);
            }
            clusters = kmeans(centers,3);
            
            
            
            vector<int> cluster_radiuses = {};
            for(cluster c : clusters){
                vector<cv::Point2i> cpoints = get<1>(c);
                vector<int> available_radiuses = {};
                for (cv::Point2i p : cpoints){
                    auto match = find(centers.begin(), centers.end(), p);
                    available_radiuses.push_back(radiuses[distance(centers.begin(),match)]);
                }
                cluster_radiuses.push_back(*max_element(available_radiuses.begin(), available_radiuses.end()));
            }
            
            for(cv::Vec3f c : circles){
                cv::Point2i p = cv::Point2i(int(c[0]), int(c[1]));
                // draw the circle center
                cv::circle( frame, p, 3, cv::Scalar(0,0,0), -1, 8, 0 );
                // draw the circle outline
                cv::circle( frame, p, int(c[2]), cv::Scalar(0,0,0), 2, 8, 0 );
            }
            
            for(cluster c : clusters){
                cv::Point2i p = get<0>(c);
                // draw the circle center
                cv::circle( frame, p, 3, cv::Scalar(255,0,0), -1, 8, 0 );
                // draw the circle outline
                cv::circle( frame, p, 40, cv::Scalar(255,0,0), 2, 8, 0 );
            }
            
            int i = 0;
            for(cluster c : clusters){
                cv::Point2i p = get<0>(c);
                // draw the circle center
                cv::circle( frame, p, 3, cv::Scalar(0,255,0), -1, 8, 0 );
                // draw the circle outline
                cv::circle( frame, p, cluster_radiuses[i++], cv::Scalar(0,255,0), 2, 8, 0 );
            }
        }
        //cv::Mat mask;
        //inRange(frame, cv::Scalar(210, 210, 210), cv::Scalar(255, 255, 255), mask);
        //frame.setTo(cv::Scalar(0, 255, 0), mask);
        cv::line(frame, cv::Point2i(0,int(frame.rows / 2)), cv::Point2i(frame.cols,int(frame.rows / 2)), cv::Scalar(0,0,255), 2);
        cv::line(frame, cv::Point2i(int(frame.cols / 2), 0), cv::Point2i(int(frame.cols / 2),frame.rows), cv::Scalar(0,0,255), 2);
        if (clusters.size() > 0){
            cv::line(frame, cv::Point2i(int(frame.cols / 2),frame.rows), cv::Point2i(get<0>(clusters[0]).x, int(frame.rows / 2)), cv::Scalar(0,0,255), 2);
            correction = degrees(asin((get<0>(clusters[0]).x - frame.cols / 2)/(frame.rows / 2)))/90*(FOV / 2);
            
        }
        cv::putText(frame, to_string(correction) + "d",cv::Point2i(10,frame.rows), cv::FONT_HERSHEY_SIMPLEX, 1,cv::Scalar(255,0,0));
        cv::imshow("frame", frame);
        if (cv::waitKey(1) == 'q'){
            break;
        }
    }
    
    cap.release();
    cv::destroyAllWindows();
    return 0;
}

