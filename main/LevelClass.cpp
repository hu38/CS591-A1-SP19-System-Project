#include "LevelClass.h"

// Append: called from a BufferClass object, adds a keyValue object to end of array

void LevelClass::sortMerge() {
    /*
    1. sort merge
    2. flush currentLevel down - delete current level's files and create a new file for next file at file_1
    */
    BufferClass bf = readFile(bufferLocation[0]);

    KeyValuePair kv1[BUFFER_SIZE] = 
    BufferClass BC;
    BC.keyValueArray = 
    BufferClass answer = new BufferClass()
    int i = 0, j = 0, k = 0;

    a = bf[0]
    b = bf[1]

    while (i < a.currentSize && j < b.currentSize){
        if a[i].key < b[j].key{
            answer.append(a[i])
        }
        else if a[i].key > b[j].key{

            answer.append(b[j])
        }
        else{
            answer.append(b[j]) // for now, we can look into it later
        }
    }

    while (i < a.currentSize){
        answer.append(a[i])
    } 

    while (j < b.currentSize) {   
        answer.append(b[i])
    }
    // Call Append into a new level with the class
    bf = bf[2:]
    return(MergedSortedLevelClass)

}

vector<KeyValuePair> LevelClass::readFile(string filename) {
    // BufferClass BC;
    
    KeyValuePair levelArray[BUFFER_SIZE];
    int key;
    string value;
    fstream newLevel;
    newLevel.open(filename);
    // int count = level * SIZE_RATIO;
    // currentLevel = stoi(string() + filename.at(7));
    // currentSize = (stoi(string() + filename.at(13)) - 1) * SIZE_RATIO;
    currentLevel = 1;
    currentSize = 0;
    // int count = 0;
    while (newLevel >> key >> value) {
        // BC.keyValueArray[currentSize] = (KeyValuePair) {key, value};
        levelArray[currentSize] = (KeyValuePair) {key, value};
        currentSize = currentSize + 1;
    }
    newLevel.close();
    vector<KeyValuePair> vec(levelArray, levelArray + BUFFER_SIZE);
    
    return vec;
}

void LevelClass::printLV() {
    for (int i=0; i< BUFFER_SIZE * SIZE_RATIO; i++) {
        cout << levelArray[i].key << " - " << levelArray[i].value << endl;
    }
}
