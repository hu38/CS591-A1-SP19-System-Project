#ifndef KeyValueClass_H
#define KeyValueClass_H
#include <iostream>
#include <time.h>
using namespace std;

class KeyValueClass {
    public:
        int key;
        int value[3];
        time_t timeStamp;
        string operation;
        int getData(int data);
        // void setData();
};

#endif
