#ifndef BufferClass_H
#define BufferClass_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h> 
#include <sys/stat.h>
#include <sys/types.h>
#include <string>
#include <sstream>
// #define GetCurrentDir getcwd // diff for windows
// #include <dirent.h>
#include <time.h>
#include <fstream> 
#include <chrono>
#include <algorithm>
#include <vector>
#include <typeinfo>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <stddef.h>
using namespace std;
#define TIMER chrono::high_resolution_clock
const int SIZE_RATIO = 4;
const int BUFFER_SIZE = 100;

struct stat info;

#pragma pack(1)
struct KeyValuePair
{   int key;
    string value;
    bool flag;
};

class BufferClass {
	public:
        vector<KeyValuePair> keyValueArray;
        int currentSize; 
        int smallest; // keyRange[0]
        int largest; // keyRange[1]
        int totalNonDup; // total non-duplicated key-value pairs

        int getCurrentSize();
        void insert(int key, string value, bool flag, int Q);
        int flushFirstLevel();
        int flushLevel(int levelNumber);
        int flushLevels(int levelNumber);
        string flushTier(int numberOfTiersInLevel1);
        void writeToFile(string filename, vector<KeyValuePair> data);

        vector<KeyValuePair> sortMerge(vector<KeyValuePair> array1, vector<KeyValuePair> array2);
        vector<KeyValuePair> readFile(string filename);
        int flush(int currentLevel);
        int explore(const char *dirname, int currentLevel);
        string GetCurrentWorkingDir();
        void printBC();
        string searchKeyInBuffer(int key);
        void sortBC();
        int checkKey(int key);

};
#endif
