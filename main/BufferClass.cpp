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
    // cout << "SIZE BEFORE "<< currentSize << endl;
    // for the first element, simply add it to position 0 and increase the current size
    if (currentSize == 0){
         keyValueArray[0] = (KeyValuePair) {key, value, flag};
         currentSize++;
    } else{
        // iterate through the entire array until finding a key larger than the new key or getting to the end
        for (int i = 0; i < currentSize; i++){
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
                currentSize++;
                break;
            // if the end of the buffer is reached, simply add the element to the end
            } else if (i == currentSize - 1){
                keyValueArray[currentSize] = (KeyValuePair) {key, value, flag};
                currentSize++;
                break;
            }
        }
    }
    // if the current size reaches the limit, flush the buffer and restart the counters
    if (currentSize == BUFFER_SIZE){
        cout << " FLUSH " << endl;
        flush();
    }
    // cout << "SIZE AFTER "<< currentSize << endl;
}

/**
 * flushes filled buffer data into the first level by creating a binary file. manifest file will be updated
 * 
 * invoked when workload requests to insert into LSM's full buffer array
 * 
 * @param void
 * @return void
 */
void BufferClass::flush() {
    string curDir = GetCurrentWorkingDir() + "/lsm_data";
    char *cstr = &curDir[0u];
    // get the current file count for level 1 from manifest file and increment it
    int nextIter = explore(cstr) + 1; //TODO: change this to retrieve from manifest file 
    string filename = string() + "lsm_data/level_1_file_" + std::to_string(nextIter) + ".txt";
    // create a file to store buffer with fstream
    // we decided to use C++'s fstream libray because of the design of our struct. with padding,
    // each of our struct contains a size of 40 ((4+4) + 8 + 8 + 8 + (1 + 7)). 
    // 1. when we use fstream, C++ can ignore the structure and directly stores values into the txt file.
    // whereas fopen and fread, C will serialize the struct for us, which inevitably adds more bytes
    // for each file. 
    // note: for our manifest file, we will instead use fopen because it'd be easier to have struct ready to go.
    // 2. C's fopen, however, does have the advantages of keeping our struct when we open saved files 
    // in the future. For our purpose, it's not needed to keep the structure because we have the manifest file
    // to log all the executed operations in LSM. TODO: YES?
    // note: if our datatypes for key and value change, the above analysis holds but not the decision.
    std::ofstream bufferFile (filename);
    for (int i=0; i < BUFFER_SIZE ; i++) {
        int key = keyValueArray[i].key;
        string value = keyValueArray[i].value;
        bool flag = keyValueArray[i].flag;
        bufferFile << key << " " << value << " " << flag << "\n";
    }
    // reset BufferClass
    currentSize = 0; //TODO: check if reset currentSize here is sufficient
    bufferFile.close();
    // we decided to use stack instead of heap memory allocation for buffer class for the following reasons:
    // 1. local variables live in stack, and buffer data will only live in local variable, instead of disk
    // 2. stackoverflow: stack's average size is 2MB (2^20 = 1,048,576), which is very difficult for buffer
    //  to exceed. (substract metafile, approx. 200B, from 2MB. Suppose our BUFFER_SIZE is 10, it'd require 
    //  each struct contains 104,838 bytes, which likely won't happen)
    // 3. heap requires malloc and free(), which slows the runtime, so it's not ideal in this particular case.
    // however, we will be using heap memory when we flush from level to level. by then, we'd like to get the 
    // actually memory location directly stored in memory instead of copying and reconverting. //TODO: maybe?
    // We will be careful about allocation and deallocation to avoid memory leak. 
    // TODO: updates level 1's information in manifest file
}

void BufferClass::flush2() {
    string curDir = GetCurrentWorkingDir() + "/lsm_data";
    char *cstr = &curDir[0u];
    // get the current file count for level 1 from manifest file and increment it
    int nextIter = explore(cstr) + 1; 
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
}

/**
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
 * checks how many files are in level 1
 * 
 * [Note: may be deprecated with manifest file]
 * @param[*dirname:required] current local directory pointer
 * @return the current number of files for level 1
 */
int BufferClass::explore(const char *dirname) {
    struct dirent *entry;
    DIR *dir = opendir(dirname);
    int cur = 0;
    if (dir == NULL) {
      return cur;
    }
    
    while ((entry = readdir(dir)) != NULL) {
        if (entry -> d_name[0] != '.') {
            string path = string(entry -> d_name);
            if (path.substr(0, 7) == "level_1") {
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

///////////////////////////////////////////experiement code////////////////////////////////////////////////////

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
