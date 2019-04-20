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

void BufferClass::insert(int key, string value) { 
    if (currentSize == 0){
         keyValueArray[0] = (KeyValuePair) {key, value};
         currentSize++;
    }
    else{
        for (int i = 0; i < currentSize; i++){
            cout << "key "  << keyValueArray[i].key << " currentSize " << currentSize << " i " << i << endl;
            if (keyValueArray[i].key == key){
                cout << "=" << endl;
                keyValueArray[i].value = value;
                break;
            }
            else if (keyValueArray[i].key > key){
                cout << ">" << endl;
                for(int j = currentSize; j >= i; j--){
                    cout << "> - up" << endl;
                    keyValueArray[j]=keyValueArray[j-1];
                    }
                keyValueArray[i] = (KeyValuePair) {key, value};
            }
            else if (i == currentSize - 1){
                cout << "===" << endl;
                keyValueArray[currentSize] = (KeyValuePair) {key, value};
            }
        }
         currentSize++;
    }

    if (currentSize == BUFFER_SIZE+1){
        flush();
    }
}

void BufferClass::printBC() {
    cout << "key  |  value" << endl;
    for (int i=0; i < BUFFER_SIZE ; i++) {
        cout << std::to_string(keyValueArray[i].key) + "\t" + keyValueArray[i].value << endl;
    }
    return;
}

bool sorter(KeyValuePair lhs, KeyValuePair rhs) {
    return lhs.key < rhs.key; 
}

void BufferClass::sortBC() {
    std::sort(keyValueArray, keyValueArray + BUFFER_SIZE, sorter);
}  


void BufferClass::flush() {
    LevelClass level;
    time_t currentTime = time(NULL); 
    string filename = ctime(&currentTime);
    std::ofstream bufferFile (filename);
    bufferFile << "currentSize | key | value" << endl;
    for (int i=0; i < BUFFER_SIZE ; i++) {
        int key = keyValueArray[i].key;
        string value = keyValueArray[i].value;
        bufferFile << i+1 << "\t\t" <<  key << "\t\t" << value << endl;
    }
    bufferFile.close();
    level.currentLevel = 1;
    level.currentSize = 0;
    level.bufferLocation[level.currentSize] = filename;
}
