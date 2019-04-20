#include "BufferClass.h"
#include "LevelClass.h"
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

void BufferClass::insert(int key, string value) {   
    if (currentSize != BUFFER_SIZE) {
        // TODO: binary search to insert
        keyValueArray[currentSize] = (KeyValuePair) {key, value};
        // time_t currentTime = time(NULL); 
        // lastUpdatedTime = ctime(&currentTime);
        // if (keyRange[1] < key) keyRange[1] = key;
        // if (keyRange[0] > key) keyRange[0] = key;
        currentSize = currentSize + 1;
    } 
    else if (currentSize == BUFFER_SIZE-1) {
        // check duplicates -> if no dup, we flush, else remove and insert
    }
    else if (currentSize == 0) {
        std::ofstream bufferFile ("buffer-data.txt");
        bufferFile << "" << std::endl;
        bufferFile.close();
    }
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

void BufferClass::sortBC() {
    std::sort(keyValueArray, keyValueArray + BUFFER_SIZE, sorter);
}  


void BufferClass::flush(LevelClass level) {
    //TODO: create file and store pointer of the file to pass to flush(). 
    //tiering and leveling
    // for each file, store a pointer and the file header stores the matadata including pointers to the next file
    // log file for all the level, can be LSM class
/*  open a new ssh file
    - pass buffer array in
    - append keyValueArray, file pointer, key range to levelArray
    close the ssh file
    clear buffer-data.txt
*/
    // f.write()
    // level.insertBuffer(data);
    // data.KeyValueParis = {};
    // data.currentSize = 0;
    // data.keyRange = {};
    // data.time_t = null;
    // data.save()
}
