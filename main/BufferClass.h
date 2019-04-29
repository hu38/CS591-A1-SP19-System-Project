#ifndef BufferClass_H
#define BufferClass_H

#include <iostream>
#include <sys/uio.h>//https://stackoverflow.com/questions/2762010/osx-sys-io-h-not-found
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
#include <typeinfo>
using namespace std;

const int BUFFER_SIZE = 5;
struct stat info;
struct KeyValuePair
{   int key;
    string value; //TODO: vary datatype? change it to char[x]!!! KeyValuePair is 40 bytes = (4+4) + 8 + 8 + 8 + (1+7).
    bool flag;
};

class BufferClass {
	public:
                //TODO: upgrade to skiplist
                KeyValuePair keyValueArray[BUFFER_SIZE];
                int currentSize;

                int getCurrentSize();
                void insert(int key, string value, bool flag);
                void flush();

                void flush2();
                int explore(const char *dirname);
                string GetCurrentWorkingDir();
                void printBC();
                string searchKeyInBuffer(int key);
                void sortBC();
                int checkKey(int key);
};
#endif
