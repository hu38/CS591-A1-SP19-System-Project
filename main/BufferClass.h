#ifndef BufferClass_H
#define BufferClass_H

#include <iostream>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
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
using namespace std;

const int SIZE_RATIO = 4;
const int BUFFER_SIZE = 1000;

struct stat info;
struct KeyValuePair
{   int key;
    string value; //TODO: vary datatype? change it to char[x]!!! KeyValuePair is 40 bytes = (4+4) + 8 + 8 + 8 + (1+7).
    bool flag;
};

class BufferClass {
	public:
                //TODO: upgrade to skiplist
                vector<KeyValuePair> keyValueArray;
                int currentSize; 
                int smallest; // keyRange[0]
                int largest; // keyRange[1]
                int totalNonDup; // total non-duplicated key-value pairs

                int getCurrentSize();
                void insert(int key, string value, bool flag, int Q);
                int flushLevel(int currentLevel);
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
