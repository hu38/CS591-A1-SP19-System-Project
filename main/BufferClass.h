#ifndef BufferClass_H
#define BufferClass_H

#include <iostream>
#include <time.h>
using namespace std;


const int BUFFER_SIZE = 4;
class BufferClass {
	public:
	    KeyValueClass keyValuePairs[BUFFER_SIZE];
	    int currentSize;
	    int keyRange[2];
	    time_t lastTimeStamp;
	    //BufferClass sort(BufferClass data);
	    //void flush(BufferClass data, LevelClass level);
	    //void append(KeyValueClass data, BufferClass bc);
	    int getCurrentSize();
	    void printBC();


};
#endif