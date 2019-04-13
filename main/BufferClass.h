#ifndef BufferClass_H
#define BufferClass_H

#include <iostream>
#include <string>
#include <time.h>
#include <fstream> 
#include <chrono>
#include <ctime>
using namespace std;


const int BUFFER_SIZE = 4;

struct KeyValuePair
{
    int key;
    string value;
};

class BufferClass {
	public:
        KeyValuePair keyValueArray[BUFFER_SIZE]; // combine keyvalueclass and bufferclass
	    int currentSize;
	    string keyRange[2];
	    string lastUpdatedTime;

        int getCurrentSize();
        void setData(int key, string value);
        string getData(int key);
        void restoreDefault();
	    //BufferClass sort(BufferClass data);
	    //void flush(BufferClass data, LevelClass level);
	    //void append(KeyValueClass data, BufferClass bc);
	    
	    // void printBC();
        // create file in the work folder and access via file pointer
};
#endif
