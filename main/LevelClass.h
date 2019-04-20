#include <iostream>
#include <BufferClass.h>
using namespace std;

const int BUFFER_SIZE = 4;
// const int MAX_SIZE = 2  ** 4;

class LevelClass {
    char bufferLocation[BUFFER_SIZE];
    int currentSize;
    int currentLevel;
};
