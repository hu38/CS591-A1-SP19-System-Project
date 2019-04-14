#include "BufferClass.h"

using namespace std;

int BufferClass::getCurrentSize() {
    return(currentSize);
}

string BufferClass::getData(int key) 
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
        keyValueArray[currentSize] = (KeyValuePair) {key, value};
        time_t currentTime = time(NULL); 
        lastUpdatedTime = ctime(&currentTime);
        if (keyRange[1] < key) keyRange[1] = key;
        if (keyRange[0] > key) keyRange[0] = key;
        currentSize = currentSize + 1;
    } 
    else if (currentSize == 0) {
        cout << "hjere" << endl;
        std::ofstream bufferFile ("buffer-data.txt");
        bufferFile << "" << std::endl;
        bufferFile.close();
    }
    // else if (currentSize == BUFFER_SIZE-1) {
    //     // flush()
    // }
    return;
}

void BufferClass::printBC() {
    cout << "key  |  value" << endl;
    for (int i=0; i < BUFFER_SIZE ; i++) {
        cout << std::to_string(keyValueArray[i].key) + "\t" + keyValueArray[i].value << endl;
    }
    return;
}

bool sorter(KeyValuePair lhs, KeyValuePair rhs) { return lhs.key < rhs.key; }

void BufferClass::sort() {
    std::sort(keyValueArray, keyValueArray + BUFFER_SIZE, sorter);
}


// void BufferClass::restoreDefault() {
//     for (int i=0; i < BUFFER_SIZE ; i++) {
        

//         // keyValueArray[i].key = null;
//         // keyValueArray[i].value = null;
//     }
// }
/*
void BufferClass::flush(LevelClass level) {
    level.insertBuffer(data);
    data.KeyValueParis = {};
    data.currentSize = 0;
    data.keyRange = {};
    data.time_t = null;
    data.save()
}
*/