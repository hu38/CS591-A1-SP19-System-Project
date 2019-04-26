#include "LevelClass.h"

// Append: called from a BufferClass object, adds a keyValue object to end of array



vector<KeyValuePair> LevelClass::sortMerge(vector<KeyValuePair> array1, vector<KeyValuePair> array2) {
/**
       @params two sorted vectors of key value pairs with no duplicates, where array1 contains older inserts/updates than array 2.
       @return a vector of key value pairs, with no duplicates.
       This function will merge two arrays of key value pairs and return a sorted array.
 */

    // Initialize vecture of result
    vector<KeyValuePair> Result;
    int i = 0, j = 0, k = 0;

    // While both arrays have elements left to iterate through, compare the next element in each array and add
    // the one with the smallest key to the result array
    while (i < array1.size() && j < array2.size()){
        if (array1[i].key < array2[j].key) {
            Result[i+j] = array1[i];
            i++;
        }
        else if (array1[i].key > array2[j].key) {
            Result[i+j] = array2[j];
            j++;
        }

        // If a duplicate is found, add the newest one and ignore the other one
        else{
            Result[i+j] = array2[i]
            i++;
            j++;
        }
    }
    
    // if only one of the two arrays have elements left, add them to the end of the result array.
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
