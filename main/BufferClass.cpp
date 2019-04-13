#include "BufferClass.h"

using namespace std;

int BufferClass::getCurrentSize() {
    return(currentSize);
}

string BufferClass::getData(string key) 
{ 
    for (int i=0; i < BUFFER_SIZE ; i++) {
        if (keyValueArray[i].key == key) {
            return keyValueArray[i].value; //TODO: refactor index based on condition
        }
    }
    return "Not Found";
} 

void BufferClass::setData(int key, string value) {   

    if (currentSize != BUFFER_SIZE) {
        keyValueArray[currentSize++] = (KeyValuePair) {key, value};
        time_t currentTime = time(NULL); 
        lastUpdatedTime = ctime(&currentTime);
        if (keyRange[1] < key) keyRange[1] = key;
        if (keyRange[0] > key) keyRange[0] = key;
    } 
    // else if (currentSize == 0 or currentSize == BUFFER_SIZE-1) {
        
    // }
    return;
}

void BufferClass::restoreDefault() {
    for (int i=0; i < BUFFER_SIZE ; i++) {
        KeyValueArray[i] = *cur;

        // keyValueArray[i].key = null;
        // keyValueArray[i].value = null;
    }
}
/*BufferClass BufferClass::sort(BufferClass data) {
    // sort based on key of operations
    SortedBufferClass = library.sort(data) // std::sort() is a sort library
    return(SortedBufferClass)
}

void BufferClass::flush(BufferClass data, LevelClass level) {
    level.insertBuffer(data);
    data.KeyValueParis = {};
    data.currentSize = 0;
    data.keyRange = {};
    data.time_t = null;
    data.save()
}

void BufferClass::append(KeyValueClass data, BufferClass bc) {
    current = bc.getCurrentSize();
    bc.keyValuePairs[current] = data;
    bc.currentSize ++;
}
*/


// void BufferClass::printBC() {
//     cout << "AHOI" << endl;
//     return;
// }
