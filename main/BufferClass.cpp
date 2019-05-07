#include "BufferClass.h"
using namespace std;

/** 
 * This function takes the key, value and flag for a new write, update or delete and inserts it into the sorted buffer with no duplicates
 * 
 * @param[key:required] key to insert
 * @param[value:required] value of the key
 * @param[flag:required] indicates if key is being deleted, a tombstone marker
 * @return no returns, simply updates the buffer 
 */
void BufferClass::insert(int key, string value, bool flag, int Q) { 
    // for the first element, simply add it to position 0 and increase the current size
    if (currentSize == 0){
        keyValueArray.push_back((KeyValuePair) {key, value, flag});
        currentSize++;
        totalNonDup++;
    } else if (currentSize <= Q) {
        // iterate through the entire array until finding a key larger than the new key or getting to the end
        for (int i = 0; i < currentSize; i++) {
            // if a duplicate is found, subsitute the new value and flag
            if (keyValueArray[i].key == key){
                keyValueArray[i].value = value;
                keyValueArray[i].flag = flag;
                break;
            // if a larger key is found, insert the new key,value pair and shift all other elements down
            } else if (keyValueArray[i].key > key){

                keyValueArray.insert(keyValueArray.begin() + i, (KeyValuePair) {key, value, flag});
                totalNonDup++;
                currentSize++;
                break;
            // if the end of the buffer is reached, simply add the element to the end
            } else if (i == currentSize - 1){
                keyValueArray.push_back((KeyValuePair) {key, value, flag});
                currentSize++;
                totalNonDup++;
                break;
            }
        }
    }
}

bool fexists(const char *filename) {
  std::ifstream ifile(filename);
  return (bool)ifile;
}
/**
 * flushes filled buffer data into the first level by creating a binary file. manifest file will be updated
 * 
 * invoked when workload requests to insert into LSM's full buffer array
 * 
 * @param[levelSize] how many pages are in a level. it should be 1 all the time in leveling.
 * @return a placeholder string for no apparent purpose
 */
int BufferClass::flushFirstLevel() {
    // cout << "S----------------------------------" << endl;
    // cout << "I'm getting the levelnumber as " << levelNumber << endl;
    vector<KeyValuePair> ret;
    // previous level
    // string prevRecordName = "lsm_data/level_" + to_string(levelNumber + 1) + "_file_1.txt";
    // cur target level
    string curRecordName = "lsm_data/level_" + to_string(1) + "_file_1.txt";
    // buffer data
    vector<KeyValuePair> bufferKV = keyValueArray;
    // cout << "bufferKV has " << bufferKV.size() << endl;
    // get the existing level 1 key-value data if there is any.
    // char *prevFile = &prevRecordName[0u];
    char *curFile = &curRecordName[0u];
    // cout << fexists(prevFile) << " --- " << fexists(curFile) << endl;
    // if both previous and presentn level files exist
    // if (fexists(prevFile) == 1 and fexists(curFile) == 1) {
    //     vector<KeyValuePair> prevKV = readFile(prevRecordName);
    //     vector<KeyValuePair> curKV = readFile(curRecordName);
    //     // sort merge existing and new data to form a new leveling data
    //     ret = sortMerge(prevKV, curKV);
    //     ret = sortMerge(ret, bufferKV);
    //     // cout << "prevKV " << prevRecordName << " has " << prevKV.size() << endl;
    //     // cout << "curKV " << curRecordName <<  " has " << curKV.size() << endl;
    // } else if (fexists(prevFile) == 1 and fexists(curFile) == 0) {
    //     vector<KeyValuePair> prevKV = readFile(prevRecordName);
    //     ret = sortMerge(prevKV, bufferKV);
    //     // cout << "prevKV " << prevRecordName << " has " << prevKV.size() << endl;
    if (fexists(curFile) == 1) {
        vector<KeyValuePair> curKV = readFile(curRecordName);
        ret = sortMerge(curKV, bufferKV);
        // cout << "curKV " << curRecordName <<  " has " << curKV.size() << endl;
    } else {
        ret = bufferKV; 
    }
    // cout << "merged ret has " << ret.size() << endl;
    // put the updated level 1 data to the original file "level_<level_number>_file_1.txt"
    ofstream bufferFile (curRecordName);
    // cout << "merged ret to " << curRecordName << endl;
    for (int i=0; i < ret.size() ; i++) {
        int key = ret[i].key;
        string value = ret[i].value;
        bool flag = ret[i].flag;
        bufferFile << key << " " << value << " " << flag << "\n";
    }
    bufferFile.close();

    // remove(prevFile);
    smallest = ret[0].key;
    largest = ret.back().key;
    // cout << "----------------------------------E" << endl;
    // cout << ret.back().key << " - " << ret[ret.size()].key << " or " << ret[ret.size() - 1].key << endl;

    return ret.size();
}

int BufferClass::flushLevels(int levelNumber) {
    // TIMER::time_point start = TIMER::now();
    vector<KeyValuePair> ret;
    vector<string> files;
    int count = 0;
    for (int i=0; i<levelNumber; i++) {
        string filename = "lsm_data/level_" + to_string(i) + "_file_1.txt";
        if (fexists(&filename[0u])) files.push_back(filename);
    }
    
    if (files.size() < 1) return ret.size();
    do {
        if (count == 0 and files.size() == 1) {   
            vector<KeyValuePair> vec1 = readFile(files[count]);
            ret = vec1;
            break;
        }
        else if (count == 0 and files.size() > 1) {
            vector<KeyValuePair> vec1 = readFile(files[count]);
            vector<KeyValuePair> vec2 = readFile(files[count + 1]);
            ret = sortMerge(vec1, vec2);
        }
        else {
            vector<KeyValuePair> vec3 = readFile(files[count]);
            ret = vec3.size() > 0 ? sortMerge(ret, vec3) : ret;
        }
        count += 1;
    } while (count < files.size());

    for (int i=0; i<=files.size(); i++) {
        remove(&files[i][0u]);
    }

    string filename = "lsm_data/level_" + to_string(levelNumber + 1) + "_file_1.txt";
    ofstream bufferFile (filename);
    // cout << "merged ret to " << curRecordName << endl;
    for (int i=0; i < ret.size() ; i++) {
        int key = ret[i].key;
        string value = ret[i].value;
        bool flag = ret[i].flag;
        bufferFile << key << " " << value << " " << flag << "\n";
    }
    bufferFile.close();

    smallest = ret[0].key;
    largest = ret.back().key;

    // TIMER::time_point ending = TIMER::now();
    // chrono::duration<double> elapsed = (ending - start);
    // cout << "each: " << elapsed.count() << endl;
    return ret.size();
}

int BufferClass::flushLevel(int levelNumber) {
    // cout << "S----------------------------------" << endl;
    // cout << "I'm getting the levelnumber as " << levelNumber << endl;
    vector<KeyValuePair> ret;
    // previous level
    string prevRecordName = "lsm_data/level_" + to_string(levelNumber + 1) + "_file_1.txt";
    // cur target level
    string curRecordName = "lsm_data/level_" + to_string(1) + "_file_1.txt";
    // buffer data
    vector<KeyValuePair> bufferKV = keyValueArray;
    // cout << "bufferKV has " << bufferKV.size() << endl;
    // get the existing level 1 key-value data if there is any.
    char *prevFile = &prevRecordName[0u];
    char *curFile = &curRecordName[0u];
    // cout << fexists(prevFile) << " --- " << fexists(curFile) << endl;
    // if both previous and presentn level files exist
    if (fexists(prevFile) == 1 and fexists(curFile) == 1) {
        vector<KeyValuePair> prevKV = readFile(prevRecordName);
        vector<KeyValuePair> curKV = readFile(curRecordName);
        // sort merge existing and new data to form a new leveling data
        ret = sortMerge(prevKV, curKV);
        ret = sortMerge(ret, bufferKV);
        // cout << "prevKV " << prevRecordName << " has " << prevKV.size() << endl;
        // cout << "curKV " << curRecordName <<  " has " << curKV.size() << endl;
    } else if (fexists(prevFile) == 1 and fexists(curFile) == 0) {
        vector<KeyValuePair> prevKV = readFile(prevRecordName);
        ret = sortMerge(prevKV, bufferKV);
        // cout << "prevKV " << prevRecordName << " has " << prevKV.size() << endl;
    } else if (fexists(prevFile) == 0 and fexists(curFile) == 1) {
        vector<KeyValuePair> curKV = readFile(curRecordName);
        ret = sortMerge(curKV, bufferKV);
        // cout << "curKV " << curRecordName <<  " has " << curKV.size() << endl;
    } else {
        ret = bufferKV; 
    }
    // cout << "merged ret has " << ret.size() << endl;
    // put the updated level 1 data to the original file "level_<level_number>_file_1.txt"
    ofstream bufferFile (curRecordName);
    // cout << "merged ret to " << curRecordName << endl;
    for (int i=0; i < ret.size() ; i++) {
        int key = ret[i].key;
        string value = ret[i].value;
        bool flag = ret[i].flag;
        bufferFile << key << " " << value << " " << flag << "\n";
    }
    bufferFile.close();

    remove(prevFile);
    smallest = ret[0].key;
    largest = ret.back().key;
    // cout << "----------------------------------E" << endl;
    // cout << ret.back().key << " - " << ret[ret.size()].key << " or " << ret[ret.size() - 1].key << endl;

    return ret.size();
}


string BufferClass::flushTier(int numberOfTiersInLevel1) {
    //cout << "FLUSHING BUFFER" << endl;
    // create the current level file
    string NewRecordName = "lsm_data/level_1_file_" + to_string(numberOfTiersInLevel1) + ".txt";
    std::ofstream bufferFile (NewRecordName);
    for (int i=0; i < currentSize ; i++) {
        int key = keyValueArray[i].key;
        string value = keyValueArray[i].value;
        bool flag = keyValueArray[i].flag;
        bufferFile << key << " " << value << " " << flag << "\n";
    }
    
    bufferFile.close();
    vector<KeyValuePair> NEWVECT;
    keyValueArray = NEWVECT;
    return NewRecordName;
}

/**
 * parses a file that stores each flushed buffer data into a vector of KeyValuePair
 * 
 * @param[filepath:required] file path of the buffer data to read. e.g.("lsm_data/level_1_file_1.txt") 
 * @return a vector of KeyValuePair with a size of BUFFER_SIZE if file exists, else 0.
 */
vector<KeyValuePair> BufferClass::readFile(string filepath) {
    vector<KeyValuePair> ret;
    int key;
    string value;
    bool flag;
    ifstream newFile;
    newFile.open(filepath);
    int count = 0;
    while (newFile >> key >> value >> flag) {
        if (count == 0) {
            ret.push_back((KeyValuePair) {key, value, flag});
            count += 1;
        }
        if (count > 0) {
            if (ret.back().key != key) {
                // cout << "not equal: " << ret.back().key << " - " << key << endl;
                ret.push_back((KeyValuePair) {key, value, flag});
                count += 1;
            }
        }
    }
    // currentSize += 1;
    newFile.close();
    // FILE *readFile; 
    // readFile = std::fopen(&filepath[0u], "r"); 
    // fread(&ret, sizeof(KeyValuePair), 3, readFile);
    // fclose(readFile);

    return ret;
}

/**
 * SAME AS LEVELCLASS.SORTMERGE except for it doesn't update levelArray
 */
vector<KeyValuePair> BufferClass::sortMerge(vector<KeyValuePair> array1, vector<KeyValuePair> array2) {
    // Initialize vecture of result
    vector<KeyValuePair> Result(array1.size() + array2.size());
    int i = 0, j = 0, k = 0;
    int duplicatecount = 0;
    
    // While both arrays have elements left to iterate through, compare the next element in each array and add
    // the one with the smallest key to the result array
    while (i < array1.size() && j < array2.size()){
        
        if (array1[i].key < array2[j].key) {
            Result[k] = array1[i];
            i++;
            k++;
        }
        else if (array1[i].key > array2[j].key) {
            Result[k] = array2[j];
            j++;
            k++;
        }
        // If a duplicate is found, add the newest one and ignore the other one
        else{
            Result[k] = array2[j];
            i++;
            k++;
            j++;
            duplicatecount++;
        }
    }
    
    // if only one of the two arrays have elements left, add them to the end of the result array.
    while (i < array1.size()){
          Result[k] = array1[i];
          i++;
          k++;
    } 
    while (j < array2.size()){
          Result[k] = array2[j];
          j++;
          k++;
    } 
    
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
 * searches a given key with binary search approach
 * 
 * this binary search is advised against fast_upper_bound3 for faster lookup in C++
 * 
 * @param[key:required] the key to search
 * @return the value of a given key in form of string if found, else empty string
 * @see https://academy.realm.io/posts/how-we-beat-cpp-stl-binary-search/
 */
string BufferClass::searchKeyInBuffer(int key) {
    vector<KeyValuePair> vec = keyValueArray;
    int size = vec.size();
    int low = 0;
    
    while (size > 0) {
        int half = size / 2; // size of first half of vec
        int other_half = size - half; // size of second half of vec
        int curIndex = low + half; // current checking pointer index in the vector
        int other_low = low + other_half; // low index of second half
        KeyValuePair tmp = vec[curIndex];
        int curKey = tmp.key;
        if (curKey == key) return tmp.value;
        size = half;
        low = curKey < key ? other_low : low;
    }
    
    return "";
}


/**
 * @depricated
 * retrives current working directory path
 * 
 * @param void
 * @return current working directory path in form of string
 */
// string BufferClass::GetCurrentWorkingDir() {
//     char buff[FILENAME_MAX];
//     GetCurrentDir( buff, FILENAME_MAX );
//     std::string current_working_dir(buff);
//     return current_working_dir;
// }

/**
 * @depricated
 * checks how many files are in level 1
 * 
 * [Note: may be deprecated with manifest file]
 * @param[*dirname:required] current local directory pointer
 * @return the current number of files for level 1
 */
// int BufferClass::explore(const char *dirname, int currentLevel) {
//     struct dirent *entry;
//     DIR *dir = opendir(dirname);
//     int cur = 1;
//     if (dir == NULL) {
//       return cur;
//     }
    
//     while ((entry = readdir(dir)) != NULL) {
//         if (entry -> d_name[0] != '.') {
//             string path = string(entry -> d_name);
//             if (path.substr(0, 7) == to_string(currentLevel)) {
//                 if (cur < path.at(13)) {
//                     cur = stoi(string() + path.at(13));
//                 }
//             }
//         }
//     }
//     closedir(dir);

//     return cur;
// }
