#pragma once
#include <string>

// define the 'signal' type
template <typename T, typename ... K>
using fsignal = T (*) (K ... );

using namespace std;

class repr{
    /*
    a 'represent' class for showing everything
    */
    public:
    virtual string _str();
};

string str(repr r){
    return r._str();
}

#define loop while(true)


