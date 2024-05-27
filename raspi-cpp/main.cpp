
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


#include "kmeans.hpp"

using namespace std;


int main(){
    cout << "Hello World!" << endl;
    
    int camidx;
    cout << "input camera index: ";
    cin >> camidx;
    auto cap = cv::VideoCapture(camidx);
    //for (const auto& dirEntry : filesystem::recursive_directory_iterator("./raspi-cpp/testimages")){
    while(true){
        //cv::Mat frame = cv::imread(dirEntry.path());
        cv::Mat frame;
        cap.read(frame);
        cv:GaussianBlur(frame,frame, cv::Size(5,3), 1);
        cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);
        
        vector<cv::Vec3f> circles;
        cv::HoughCircles(frame, circles, cv::HOUGH_GRADIENT, 1, 10, 80, 40, 15, 100);
        
        if (circles.size() > 0){
            vector<cv::Point2i> centers = {};
            vector<int> radiuses = {};
            for(cv::Vec3f c : circles){
                centers.push_back(cv::Point2i(c[0],c[1]));
                radiuses.push_back(c[2]);
            }
            vector<cluster> clusters = kmeans(centers,3);
            
            cv::cvtColor(frame, frame, cv::COLOR_GRAY2BGR);
            
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
            cout << frame.at(10,10);// = cv::Scalar(0,255,0);
        }
        
        cv::imshow("frame", frame);
        if (cv::waitKey(1) == 'q'){
            break;
        }
    }
    
    cap.release();
    cv::destroyAllWindows();
    return 0;
}

