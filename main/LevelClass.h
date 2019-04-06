#include <iostream>
using namespace std;

const int BUFFER_SIZE = 4;
const int MAX_SIZE = 2  ** BUFFER_SIZE;

class LevelClass {
    int sortedBufferClass[BUFFER_SIZE];
    char bufferLocation;
    int currentSize;
    int currentLevel;
};


int main() {
    cout << "This is a key value class";
    return 0;
}