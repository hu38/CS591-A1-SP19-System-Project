#ifndef BufferClass_H
#define BufferClass_H

#include <iostream>
#include <sys/uio.h> // https://stackoverflow.com/questions/2762010/osx-sys-io-h-not-found
#include <string.h>
#include <string>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#define GetCurrentDir getcwd // diff for windows
#include <dirent.h>
#include <time.h>
#include <fstream> 
#include <chrono>
#include <ctime>
#include <algorithm>
#include <vector>
#include "LevelClass.h"
using namespace std;


const int BUFFER_SIZE = 5;
struct stat info;
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
        void insertKV(int key, string value);
        bool updateKV(int key, string value);
        bool deleteKV(int key);
        string getData(int key);
	    void flush();
        int explore(const char *dirname);
        string GetCurrentWorkingDir();
        void printBC();
        void sortBC();
        int searchKey(int key);
};
#endif
