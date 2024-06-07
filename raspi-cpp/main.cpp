
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
#include <numeric>
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
        cv::Mat frame, blurred, entropy;
        cap.read(frame);
        cv::GaussianBlur(frame,frame, cv::Size(3,3), 1);
        cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);
        
        cv::GaussianBlur(frame, blurred, cv::Size(31,31), 2);
        
        //cv::Mat f;
        //cv::inRange(frame, cv::Scalar(0, 0, 0), cv::Scalar(100, 100, 100), f);
        //f.copyTo(frame(cv::Rect(0,0,f.cols, f.rows)));
        
        entropy = frame - blurred;
        cv::inRange(entropy, cv::Scalar(3, 3, 3), cv::Scalar(255, 255, 255), entropy);
        cv::rectangle(entropy, cv::Point2i(0,0), cv::Point2i(entropy.cols, int(entropy.rows*2/5)), cv::Scalar(0,0,0), cv::FILLED);
        
        cv::erode(entropy, entropy, 2);
        cv::dilate(entropy, entropy, 2);
        cv::erode(entropy, entropy, 2);
        cv::dilate(entropy, entropy, 2);
        cv::Mat blobs, centroids, stats;
        cv::connectedComponentsWithStats(entropy, blobs, stats, centroids, 8);
        
        // get sizes out of the stats matrix
        vector<cv::Scalar> sizes{};
        for(int i = 0; i < stats.rows; i++){
            sizes.push_back(stats.at<int>(int(i), stats.cols-1));
        }
        sizes.erase(sizes.begin()); // avoid the background
        
        cv::Mat entropy2(entropy.rows, entropy.cols, CV_8UC1, cv::Scalar(0,0,0));
        for(size_t i = 0; i < sizes.size(); i++){
            if(sizes[i][0] >= 50){
                cv::Mat m;
                cv::inRange(blobs, cv::Scalar(i+1), cv::Scalar(i+1), m);
                entropy2.setTo(cv::Scalar(255,255,255), m);
            }
        }
        //cv::Mat mask;
        //cv::inRange(frame, cv::Scalar(100, 100, 100), cv::Scalar(255, 255, 255), mask);
        //entropy.setTo(cv::Scalar(0,0,0), mask);
        
        vector<cv::Vec3f> circles;
        cv::HoughCircles(entropy2, circles, cv::HOUGH_GRADIENT, 1, 10, 54, 26, 15, 150);
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
            
            int s = clusters.size();
            int i = 0;
            for(int a = 0; a < s; a++){
                cluster c = clusters[i];
                if(get<0>(c).y < frame.rows/2){
                    clusters.erase(clusters.begin() + i);
                }
                else{
                    i++;
                }
            }
            
            
            vector<int> cluster_radiuses = {};
            for(cluster c : clusters){
                vector<cv::Point2i> cpoints = get<1>(c);
                vector<int> available_radiuses = {};
                for (cv::Point2i p : cpoints){
                    auto match = find(centers.begin(), centers.end(), p);
                    available_radiuses.push_back(radiuses[distance(centers.begin(),match)]);
                }
                //cluster_radiuses.push_back(*(available_radiuses.begin(), available_radiuses.end()));
                cluster_radiuses.push_back(int(accumulate(available_radiuses.begin(), available_radiuses.end(), 0)/available_radiuses.size()));
            }

            cout << frame.cols << " " << frame.rows << "\n";
            for(size_t i = 0; i < clusters.size(); i++){
                bool valid = 1;
                const float diff = radiuses[i]/3*2;
                const int y = get<0>(clusters[i]).y;
                for(int j = -2; j < 3; j++){
                    if(j==0){continue;};
                    const int x = get<0>(clusters[i]).x + copysign(radiuses[i],j) + diff * j;
                    if(!(frame.cols>x and x>0)){continue;};
                    if(frame.at<cv::Scalar>(int(x), int(y))[0] > 200){
                        cv::circle(frame, cv::Point2i(x, y), 3, cv::Scalar(0,0,255), -1, 8, 0);
                        valid = 0;
                        break;                    
                    }
                    cout << get<0>(clusters[i]).x << " " << get<0>(clusters[i]).y << "\n";
                    //cout << x << " " << circles[i][1] + radiuses[i]*2 << "\n";
                }
                if(!valid){
                    cout << get<0>(clusters[i]).x << " " << get<0>(clusters[i]).y << "    is not valid" << "\n";
                };
            }
            
            for(cv::Vec3f c : circles){
                cv::Point2i p = cv::Point2i(int(c[0]), int(c[1]));
                // draw the circle center
                cv::circle( frame, p, 3, cv::Scalar(0,0,0), -1, 8, 0 );
                // draw the circle outline
                cv::circle( frame, p, int(c[2]), cv::Scalar(0,0,0), 2, 8, 0 );
            }
            
            i = 0;
            for(cluster c : clusters){
                cv::Point2i p = get<0>(c);
                // draw the circle center
                cv::circle( frame, p, 3, cv::Scalar(255,0,0), -1, 8, 0 );
                // draw the circle outline
                cv::circle( frame, p, cluster_radiuses[i++], cv::Scalar(255,0,0), 2, 8, 0 );
            }
        }
        //cv::Mat mask;
        //inRange(frame, cv::Scalar(210, 210, 210), cv::Scalar(255, 255, 255), mask);
        frame.setTo(cv::Scalar(0, 255, 0), entropy2);
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

