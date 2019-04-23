#ifndef LevelClass_H
#define LevelClass_H

#include <iostream>
#include "BufferClass.h"

using namespace std;

const int SIZE_RATIO = 4;
// const int MAX_SIZE = 2  ** 4;

class LevelClass {
    public:
        string bufferLocation[SIZE_RATIO];
        KeyValuePair levelArray[BUFFER_SIZE * SIZE_RATIO];
        int currentSize;
        int currentLevel;

        void sortMerge();
        void readFile(string filename);
        void printLV();
};

#endif

