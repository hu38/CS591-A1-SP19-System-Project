#ifndef BufferClass_H
#define BufferClass_H

#include <iostream>
#include <string>
#include <time.h>
#include <fstream> 
#include <chrono>
#include <ctime>
#include <algorithm>
// #include <json.h>
using namespace std;


const int BUFFER_SIZE = 4;

struct KeyValuePair
{
    int key;
    string value;
};

class BufferClass {
	public:
        KeyValuePair keyValueArray[BUFFER_SIZE]; // ensure key and value share a relationship
	    int currentSize;
	    // int keyRange[2]; wait for Fence Pointer
	    // string lastUpdatedTime;

        int getCurrentSize();
        void insert(int key, string value);
        string getData(int key);
        void update(int key, string value);
        void deleteData(int key);
	    void sortBC();
	    void flush(LevelClass level);
	    
	    void printBC();
};
#endif
