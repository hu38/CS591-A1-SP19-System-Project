using namespace std;
#include <iostream>
#include <time.h>

const int BUFFER_SIZE = 4;
class BufferClass {
    KeyValueClass keyValuePairs[BUFFER_SIZE];
    int currentSize;
    int keyRange[2];
    time_t lastTimeStamp;
};

int main() 
{
    
    cout << "This is a key value class";
    return 0;
}