using namespace std;
#include <iostream>
#include <time.h>

const int BUFFER_SIZE = 4;
class BufferClass {
    KeyValueClass keyValuePairs[BUFFER_SIZE]; // combine keyvalueclass and bufferclass
    int currentSize;
    int keyRange[2];
    time_t lastTimeStamp;
};
 // create file in the work folder and access via file pointer