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
void BufferClass::insert(int key, string value, bool flag) { 
    // for the first element, simply add it to position 0 and increase the current size
    if (currentSize == 0){
        keyValueArray[0] = (KeyValuePair) {key, value, flag};
        currentSize++;
        totalNonDup++;
    } else if (currentSize < BUFFER_SIZE) {
        // iterate through the entire array until finding a key larger than the new key or getting to the end
        for (int i = 0; i < currentSize; i++) {
            // if a duplicate is found, subsitute the new value and flag
            if (keyValueArray[i].key == key){
                keyValueArray[i].value = value;
                keyValueArray[i].flag = flag;
                break;
            // if a larger key is found, insert the new key,value pair and shift all other elements down
            } else if (keyValueArray[i].key > key){
                for(int j = BUFFER_SIZE-1; j > i; j--){
                    keyValueArray[j]=keyValueArray[j-1];
                    }
                keyValueArray[i] = (KeyValuePair) {key, value, flag};
                totalNonDup++;
                currentSize++;
                break;
            // if the end of the buffer is reached, simply add the element to the end
            } else if (i == currentSize - 1){
                keyValueArray[currentSize] = (KeyValuePair) {key, value, flag};
                totalNonDup++;
                currentSize++;
                break;
            }
        }
    } else {
        currentSize = 0;
        totalNonDup = 0;
        // if the current size reaches the limit, flush in driver
    }
}

/**
 * flushes filled buffer data into the first level by creating a binary file. manifest file will be updated
 * 
 * invoked when workload requests to insert into LSM's full buffer array
 * 
 * @param[levelSize] how many pages are in a level. it should be 1 all the time in leveling.
 * @return a placeholder string for no apparent purpose
 */
bool fexists(const char *filename) {
  std::ifstream ifile(filename);
  return (bool)ifile;
}
int BufferClass::flushLevel(int levelNumber) {

    vector<KeyValuePair> ret;
    // previous level
    string prevRecordName = "lsm_data/level_" + to_string(levelNumber - 1) + "_file_1.txt";
    // cur target level
    string curRecordName = "lsm_data/level_" + to_string(levelNumber) + "_file_1.txt";
    // buffer data
    vector<KeyValuePair> bufferKV(keyValueArray, keyValueArray + BUFFER_SIZE);
    // cout << "bufferKV has " << bufferKV.size() << endl;
    // get the existing level 1 key-value data if there is any.
    char *prevFile = &prevRecordName[0u];
    char *curFile = &curRecordName[0u];
    // cout << fexists(prevFile) << " --- " << fexists(curFile) << endl;
    // if both previous and presentn level files exist
    if (fexists(prevFile) and fexists(curFile)) {
        vector<KeyValuePair> prevKV = readFile(prevRecordName);
        vector<KeyValuePair> curKV = readFile(curRecordName);
        // sort merge existing and new data to form a new leveling data
        ret = sortMerge(prevKV, curKV);
        ret = sortMerge(ret, bufferKV);
        // cout << "prevKV " << prevRecordName << " has " << prevKV.size() << endl;
        // cout << "curKV " << curRecordName <<  " has " << curKV.size() << endl;
    } else if (fexists(prevFile) and !fexists(curFile)) {
        vector<KeyValuePair> prevKV = readFile(prevRecordName);
        ret = sortMerge(prevKV, bufferKV);
        // cout << "prevKV " << prevRecordName << " has " << prevKV.size() << endl;
    } else if (!fexists(prevFile) and fexists(curFile)) {
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
    // for (int i=0 ; i < ret.size(); i++) {
    //     cout << "readFile ret has key " << to_string(ret[i].key) << " - " << ret[i].value << endl;
    // }
    smallest = ret[0].key;
    largest = ret.back().key;
    // cout << ret.back().key << " - " << ret[ret.size()].key << " or " << ret[ret.size() - 1].key << endl;
    
    return ret.size();
}


string BufferClass::flushTier(int numberOfTiersInLevel1) {
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
        ret.push_back((KeyValuePair) {key, value, flag});
        currentSize += 1;
        count += 1;
    }
    newFile.close();

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
    vector<KeyValuePair> vec (keyValueArray, keyValueArray + BUFFER_SIZE);
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
string BufferClass::GetCurrentWorkingDir() {
    char buff[FILENAME_MAX];
    GetCurrentDir( buff, FILENAME_MAX );
    std::string current_working_dir(buff);
    return current_working_dir;
}

/**
 * @depricated
 * checks how many files are in level 1
 * 
 * [Note: may be deprecated with manifest file]
 * @param[*dirname:required] current local directory pointer
 * @return the current number of files for level 1
 */
int BufferClass::explore(const char *dirname, int currentLevel) {
    struct dirent *entry;
    DIR *dir = opendir(dirname);
    int cur = 1;
    if (dir == NULL) {
      return cur;
    }
    
    while ((entry = readdir(dir)) != NULL) {
        if (entry -> d_name[0] != '.') {
            string path = string(entry -> d_name);
            if (path.substr(0, 7) == to_string(currentLevel)) {
                if (cur < path.at(13)) {
                    cur = stoi(string() + path.at(13));
                }
            }
        }
    }
    closedir(dir);

    return cur;
}

/**
 * prints out the entire buffer class
 * 
 * @param void
 * @return void
 */
void BufferClass::printBC() {
    cout << "__________________" << endl;
    cout << "| key   |  value |" << endl;
    for (int i=0; i < BUFFER_SIZE ; i++) {
        cout << "|" << to_string(keyValueArray[i].key) + "\t" <<  "|" << keyValueArray[i].value + "\t" <<  " |" << endl;
    }
    cout << "-----------------" << endl;
    return;
}

///////////////////////////////////////////experiement code////////////////////////////////////////////////////
void BufferClass::writeToFile(string filename, vector<KeyValuePair> data) {
    // ofstream newFile;
    // newFile.open(filename, ios::out | ios::binary);
    // newFile.write(reinterpret_cast<char*>(&data), sizeof(data));
    // newFile.close();
    std::ofstream newFile (filename);
    for (int i=0; i < data.size() ; i++) {
        int key = data[i].key;
        string value = data[i].value;
        bool flag = data[i].flag;
        newFile << key << " " << value << " " << flag << "\n";
    }
    newFile.close();
}

bool sorter(KeyValuePair lhs, KeyValuePair rhs) {
    // comparator for KeyValuePair struct. compare based on key by asec order
    return lhs.key < rhs.key; 
}

int BufferClass::checkKey(int key) {
    // binary search function accommadated for an array of struct
    std::vector<KeyValuePair> vec(keyValueArray, keyValueArray + BUFFER_SIZE);
    return std::binary_search(
        vec.begin(),
        vec.end(),
        (KeyValuePair) {key, },
        sorter
    );
}

void BufferClass::sortBC() {
    std::sort(keyValueArray, keyValueArray + BUFFER_SIZE, sorter);
}

int BufferClass::getCurrentSize() {
    return(currentSize);
}

/**
 * NOT IN USE
 */
int BufferClass::flush(int currentLevel) {
    string curDir = GetCurrentWorkingDir() + "/lsm_data";
    char *cstr = &curDir[0u];
    // get the current file count for level 1 from manifest file and increment it
    int nextIter = explore(cstr, currentLevel) + 1; 
    // int nextIter = currentSize + 1;
    string filename = string() + "lsm_data/level_1_file_" + std::to_string(nextIter) + ".txt";
    // create a file to store buffer
    FILE *outfile; 
    outfile = fopen("lsm_data/level_1_file_2.txt", "w"); 
    if (outfile == NULL) 
    { 
        fprintf(stderr, "\nError opend file\n"); 
        exit (1); 
    }
    fwrite(&keyValueArray, sizeof(KeyValuePair), BUFFER_SIZE, outfile); 
    // reset BufferClass
    currentSize = 0;
    fclose (outfile);

    return nextIter;
}
