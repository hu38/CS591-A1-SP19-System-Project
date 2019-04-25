#include "LevelClass.h"

// Append: called from a BufferClass object, adds a keyValue object to end of array

void LevelClass::sortMerge() {
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
    vector<KeyValuePair> c;
    int i = 0, j = 0, k = 0;

    // while (i < a.size() && j < b.size()){
    //     if a.at(i).key < b.at(j).key{
    //         answer.append(a.at(i))
    //     }
    //     else if a.at(i).key > b.at(j).key{

    //         answer.append(b.at(j))
    //     }
    //     else{
    //         answer.append(b.at(j)) // for now, we can look into it later
    //     }
    // }

    // while (i < a.size()){
    //     answer.append(a.at(i))
    // } 

    // while (j < b.size()) {   
    //     answer.append(b.at(i))
    // }
    // // Call Append into a new level with the class
    // bf = bf[2:]
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
