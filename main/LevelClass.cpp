#include "LevelClass.h"

/**
 * This function will merge two arrays of key value pairs and return a sorted array.
 * 
 * @params two sorted vectors of key value pairs with no duplicates, where array1 contains older inserts/updates than array 2.
 * @return a vector of key value pairs, with no duplicates.
 */
//TODO: a better method to update levelArray
vector<KeyValuePair> LevelClass::sortMerge(vector<KeyValuePair> array1, vector<KeyValuePair> array2) {
    // Initialize vecture of result
    vector<KeyValuePair> Result(array1.size() + array2.size());
    int i = 0, j = 0, k = 0;
    int duplicatecount = 0;
    
    // While both arrays have elements left to iterate through, compare the next element in each array and add
    // the one with the smallest key to the result array
    while (i < array1.size() && j < array2.size()){
        
        if (array1[i].key < array2[j].key) {
            Result[k] = array1[i];
            levelArray[k] = array1[i];
            i++;
            k++;
        }
        else if (array1[i].key > array2[j].key) {
            Result[k] = array2[j];
            levelArray[k] = array2[j];
            j++;
            k++;
        }
        // If a duplicate is found, add the newest one and ignore the other one
        else{
            Result[k] = array2[j];
            levelArray[k] = array2[j];
            i++;
            k++;
            j++;
            duplicatecount++;
        }
    }
    
    // if only one of the two arrays have elements left, add them to the end of the result array.
    while (i < array1.size()){
          Result[k] = array1[i];
          levelArray[k] = array1[i];
          i++;
          k++;
    } 
    while (j < array2.size()){
          Result[k] = array2[j];
          levelArray[k] = array2[j];
          j++;
          k++;
    } 
    //TODO: this is yet to be optimized, but it works, yay!
    if (duplicatecount > 0) {
        vector<KeyValuePair> finalRes(Result.size() - duplicatecount); 
        for (int q = 0; q < finalRes.size(); q++){
            finalRes[q] = Result[q];
        }
        return finalRes;
    } else {
        return Result;
    }

}

/**
 * sorts the entire level after it's full by invokeing sortMerge()
 * 
 * @param void
 * @return void
 */
vector<KeyValuePair> LevelClass::tiering() {
    int count = 0;
    vector<KeyValuePair> tmp;
    while (count != SIZE_RATIO + 1) {
        if (count == 0) {
            vector<KeyValuePair> vec1 = readFile(filenameList[count]);
            vector<KeyValuePair> vec2 = readFile(filenameList[count + 1]);
            tmp = sortMerge(vec1, vec2);
            count = 2;
        }
        else {
            vector<KeyValuePair> vec3 = readFile(filenameList[count]);
            tmp = tmp.size() > 0 ? sortMerge(tmp, vec3) : tmp;
            count += 1;
        }
    }

    return tmp;
}

/**
 * sorts whenever a new tier comes into the level by invoking sortMerge()
 * 
 * @param void
 * @return void
 */
void LevelClass::leveling() {
    readFile(filenameList[currentSize / BUFFER_SIZE]);
    vector<KeyValuePair> newTier = readFile(filenameList[currentSize / BUFFER_SIZE]);
    vector<KeyValuePair> curTiers(levelArray, levelArray + currentSize);
    sortMerge(curTiers, newTier);
}

/**
 * parses a file that stores each flushed buffer data into a vector of KeyValuePair
 * 
 * @param[filepath:required] file path of the buffer data to read. e.g.("lsm_data/level_1_file_1.txt") 
 * @return a vector of KeyValuePair with a size of BUFFER_SIZE if file exists, else 0.
 */
vector<KeyValuePair> LevelClass::readFile(string filepath) {
    int totalSize = 0;
    KeyValuePair tmp[BUFFER_SIZE];
    int key;
    string value;
    bool flag;
    fstream newLevel;
    newLevel.open(filepath);
    int count = 0;
    while (newLevel >> key >> value >> flag) {
        tmp[count] = (KeyValuePair) {key, value, flag};
        levelArray[currentSize] = (KeyValuePair) {key, value, flag};
        currentSize += 1;
        count += 1;
        totalSize = count;
    }
    vector<KeyValuePair> vec(tmp, tmp + totalSize);
    // newLevel.close();    

    return vec;
}

/**
 * fills filenameList attribute for a given class, since all filename follows 
 * the template "level_<level number>_file_<array index>.txt" under lsm_data/ directory
 * 
 * @param void
 * @return void 
 */
void LevelClass::generateFilenameList() {
    string basename = "lsm_data/level_";
    string level = to_string(currentLevel);
    string file_ = "_file_";
    string extension = ".txt";
    for (int i=0; i< SIZE_RATIO; i++) {
        filenameList.push_back(basename + level + file_ + to_string(i+1) + extension);
    }
}

/**
 * prints out the entire level class
 * 
 * @param void
 * @return void
 */
void LevelClass::printLV() {
    for (int i=0; i< BUFFER_SIZE * SIZE_RATIO; i++) {
        cout << levelArray[i].key << " - " << levelArray[i].value << endl;
    }
}

/** 
 * @depricated
 * invokes readFile for all data files for a given level
 * 
 * @param void
 * @return void
 */
void LevelClass::combineLevelArrays() {
    for (int i=0; i< SIZE_RATIO; i++) {
        cout << "iteration: " << i << ", filename: " << filenameList[i] << endl;
        readFile(filenameList[i]);
    }
}
