#include <opencv2/core/types.hpp>
#include <opencv2/opencv.hpp>
#include <vector>
#include <tuple>
#include <cstdint>
#include <cstdlib>
#include <numeric>
#include <iostream>
#include <algorithm>

#include "kmeans.hpp"

using namespace std;

cv::Point2i get_random_point(vector<cv::Point2i> p){
    uint16_t sel = rand() % p.size();
    return p[sel];
}

uint8_t get_nearest_point(cv::Point2i point, vector<cv::Point2f> others){
    uint8_t idx = 0;                    // the current index of maximal difference
    double diff_value = 10000.;         // the value
    uint16_t i = 0;
    for(cv::Point2i p : others){
        double d = cv::norm(point - p);
        if (d < diff_value){
            diff_value = d;
            idx = i;
        }
        i++;
    }
    return idx;
}

/*template<class _Tp, class _Alloc>
std::ostream &operator<<(std::ostream &os, const std::vector<cv::Point_<_Tp>> &input){
    os << "[";
    for (auto const &i: input) {
        os << i << " ";
    }
    os << "]";
    return os;
}*/

vector<cluster> kmeans(vector<cv::Point2i> points, uint8_t k){
    // create random cluster point
    vector<cv::Point2f> clusterpoints = {get_random_point(points)};
    
    //distribute other cluster points so that they have a maximal distance to all existing ones
    vector<cv::Point2i> available = points; // create a copy of all points    
    for(uint8_t i = 0; i < k-1; i++){
        uint8_t idx = 0;                    // the current index of maximal difference
        double diff_value = 10000.;         // the value
        if(available.size() < 1){           // to few circles
            break;
        }
        for(cv::Point2i p : available){
            vector<double> distances = {};
            for(cv::Point2i p2 : available){
                if(p != p2){
                    distances.push_back(cv::norm(p - p2)); // calculate euclidean distance
                }
            }
            double d = accumulate(distances.begin(), distances.end(), 0);
            if (d > diff_value){
                diff_value = d;
                idx = i;
            }
        }
        clusterpoints.push_back(available[idx]);
        available.erase(available.begin() + idx);
    }
    
    uint16_t i = 0;
    vector<cluster> clusters_old = {};
    while(true){
        cout << i++ << ">> clusterpoints: " << clusterpoints << endl;
        vector<cluster> clusters = {};
        
        for(cv::Point2f p : clusterpoints){
            clusters.push_back(make_tuple(p, vector<cv::Point2i>({})));
        }
        
        // distribute Points
        for(cv::Point2i p : points){
            get<1>(clusters[get_nearest_point(p, clusterpoints)]).push_back(p);
        }
        clusterpoints = {};
        // calculate new clusters
        for(cluster c : clusters){
            if (get<1>(c).size() > 0){
                cv::Point2f centroid = cv::Point2f(
                    accumulate(
                        get<1>(c).begin(),
                        get<1>(c).end(),
                        cv::Point2i(0,0)
                    ))/float(get<1>(c).size());
                    
                clusterpoints.push_back(centroid);
            }
            
        }
        
        if (clusters_old == clusters){
            cout << "merging centroids..." << endl;
            
            for(uint8_t i = 0; i < 3; i++){
                vector<cv::Point2f> centroids = {};
                vector<cluster> clusters_new = {};
                
                for(cluster c : clusters){
                    cv::Point2f p = get<0>(c);
                    vector<cv::Point2i> pl = get<1>(c);
                    
                    for(cluster c2 : clusters){
                        cv::Point2f p2 = get<0>(c2);
                        vector<cv::Point2i> p2l = get<1>(c2);
                        
                        if(cv::norm(p - p2) < 60 && p != p2){
                            cv::Point2f target = (p*int(pl.size())+p2*int(p2l.size()))/int(pl.size()+p2l.size());
                            if (count(centroids.begin(), centroids.end(), target) < 1){
                                centroids.push_back(target);
                                
                                // extend a vector
                                pl.reserve(pl.size() + distance(p2l.begin(), p2l.end()));
                                pl.insert(pl.end(), p2l.begin(), p2l.end());

                                
                                cluster data = make_tuple(target, pl);
                                clusters_new.push_back(data);
                                
                                
                            }
                            goto here;
                        }
                    }
                    clusters_new.push_back(c);
                    here:;
                    
                }
                clusters = clusters_new;
            }
            
            return clusters;
        }
        clusters_old = clusters;
    }
}

