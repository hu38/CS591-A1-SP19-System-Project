#include "LevelClass.h"

// Append: called from a BufferClass object, adds a keyValue object to end of array





vector<KeyValuePair> LevelClass::sortMerge(vector<KeyValuePair> array1, vector<KeyValuePair> array2) {
    /*
    1. sort merge
    2. flush currentLevel down - delete current level's files and create a new file for next file at file_1
    */
//    BufferClass bf = readFile(bufferLocation[0]);
//    bf.printBC();
//    KeyValuePair kv1[BUFFER_SIZE] = 
//    BufferClass BC;
//    BC.keyValueArray = 
    // BufferClass answer = new BufferClass()
    vector<KeyValuePair> Result;
    int i = 0, j = 0, k = 0;

    while (i < array1.size() && j < array2.size()){
        if (array1[i].key < array2[j].key) {
            Result[i+j] = array1[i];
            i++;
        }
        else if (array1[i].key > array2[j].key) {
            Result[i+j] = array2[j];
            j++;
        }

        else{
            Result[i+j] = array2[i]
            i++;
            j++;
        }
    }
    
    while (i < array1.size()){
          Result[i+j] = array1[i];
          i++;
    } 

    while (j < array2.size()){
          Result[i+j] = array2[j];
          j++;
    } 

    return Result;
}

vector<KeyValuePair> LevelClass::readFile(string filename) {
    KeyValuePair tmp[BUFFER_SIZE];
    int key;
    string value;
    fstream newLevel;
    newLevel.open(filename);
    currentLevel = stoi(string() + filename.at(15));
    currentSize = (stoi(string() + filename.at(22)) - 1) * BUFFER_SIZE;
    int count = 0;
    while (newLevel >> key >> value) {
        tmp[count] = (KeyValuePair) {key, value};
        levelArray[currentSize] = (KeyValuePair) {key, value};
        currentSize += 1;
        count += 1;
    }
    vector<KeyValuePair> vec(tmp, tmp + BUFFER_SIZE);
    newLevel.close();    

    return vec;
}

void LevelClass::generateFilenameList() {
    string basename = "lsm_data/level_";
    string level = to_string(currentLevel);
    string file_ = "_file_";
    string extension = ".txt";
    for (int i=0; i< SIZE_RATIO; i++) {
        filenameList[i] = basename + level + file_ + to_string(i+1) + extension;
    }
}

void LevelClass::combineLevelArrays() {
    for (int i=0; i< SIZE_RATIO; i++) {
        cout << "iteration: " << i << ", filename: " << filenameList[i] << endl;
        readFile(filenameList[i]);
    }
}

void LevelClass::printLV() {
    for (int i=0; i< BUFFER_SIZE * SIZE_RATIO; i++) {
        cout << levelArray[i].key << " - " << levelArray[i].value << endl;
    }
}
