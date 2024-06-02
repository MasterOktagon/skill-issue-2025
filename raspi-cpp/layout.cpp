
#include <iostream>
#include <mutex>
#include <threads>
#include <vector>
#include <string>
#include <cstdint>

#define _PROGRAM_END 0

using namespace std;


//data transfer between threads
mutex data;
vector<int16_t> sensor_data = {};
vector<string> reports;

mutex tasks;
vector<int> tasks = {};

int main(){
    // use log files to track output to acces it later
    freopen("out.log", stdout);
    freopen("errors.log", stderr);

    //Initialize Systems
    init();
    
    
    // create I2C Handler
    thread i2c(i2c_handler);
    i2c.detach;
    
    // loop
    while(true){
        analyze_image();
        
    
    }


    return _PROGRAM_END;
}





