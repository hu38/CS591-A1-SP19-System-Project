#ifndef LevelClass_H
#define LevelClass_H

#include <sstream>
#include "BufferClass.h"

using namespace std;

const int SIZE_RATIO = 4;
// const int MAX_SIZE = 2  ** 4;

class LevelClass {
    public:
        string filenameList[SIZE_RATIO];
        KeyValuePair levelArray[BUFFER_SIZE * SIZE_RATIO];
        int currentSize;
        int currentLevel;

        void sortMerge();
        vector<KeyValuePair> readFile(string filename);
        void printLV();
        void generateFilenameList();
        void combineLevelArrays();
};

#endif

