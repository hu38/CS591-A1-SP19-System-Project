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

// bool BufferClass::deleteKV(int key) {
//     // return true if key marked as DELETED, else false
//     for (int i = 0; i < currentSize; i++) {
//         if (keyValueArray[i].key == key) {
//             keyValueArray[i].value = "DELETED";
//             return true;
//         }  
//     }
//     return false;
// }

// bool BufferClass::updateKV(int key, string value) {
//     // update dup key's value if key exists and return true. if no dups, return false
//     for (int i = 0; i < currentSize; i++) {
//         // check if key is in the array to replace with new value
//         if (keyValueArray[i].key == key) {
//             keyValueArray[i].value = value;
//             return true;
//         }  
//     }
//     return false;
// }
void BufferClass::insert(int key, string value, bool flag) { 
/*
    @params a key, value and a deleted flag
    @return no returns, simply updates the buffer
    This function takes the key, value and flag for a new write, update or delete and inserts it into the sorted buffer with no duplicates
*/
    cout << "SIZE BEFORE "<< currentSize << endl;
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

    cout << "SIZE AFTER "<< currentSize << endl;
}

void BufferClass::printBC() {
    // print current buffer class key-value pairs
    cout << "__________________" << endl;
    cout << "| key   |  value |" << endl;
    for (int i=0; i < BUFFER_SIZE ; i++) {
        cout << "|" << std::to_string(keyValueArray[i].key) + "\t" <<  "|" << keyValueArray[i].value + "\t" <<  " |" << endl;
    }
    cout << "-----------------" << endl;
    return;
}

bool sorter(KeyValuePair lhs, KeyValuePair rhs) {
    // comparator for KeyValuePair struct. compare based on key by asec order
    return lhs.key < rhs.key; 
}

int BufferClass::searchKey(int key) {
    // binary search function accommadated for an array of struct
    sortBC();
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

void BufferClass::flush() {
    // LevelClass level;
    // time_t currentTime = time(NULL); 
    // string filename = ctime(&currentTime);

    // dynamically generate filename as level_<level_id>_file_<file_id>.txt
    string curDir = GetCurrentWorkingDir() + "/lsm_data";
    char *cstr = &curDir[0u];
    // if /lsm_data doesn't exist, create one
    if (stat (curDir.c_str(), &info) != 0) {
        mkdir(cstr, S_IRWXU);
    }
    // get the current file count and increment it
    int nextIter = explore(cstr) + 1;
    string filename = string() + "lsm_data/level_1_file_" + std::to_string(nextIter) + ".txt";
    // create a file to store buffer
    std::ofstream bufferFile (filename);
    // sort, store and print data written into the file
    sortBC();
    // bufferFile << "key   | value" << endl;
    for (int i=0; i < BUFFER_SIZE ; i++) {
        int key = keyValueArray[i].key;
        string value = keyValueArray[i].value;
        bufferFile << key << "\t\t" << value << endl;
        keyValueArray[i].key = 100000;
        keyValueArray[i].value = "";
    }
    currentSize = 0;
    bufferFile.close();
    // don't think it's needed
    // level.currentLevel = 1;
    // level.bufferLocation[nextIter] = filename;
}

string BufferClass::GetCurrentWorkingDir() {
  char buff[FILENAME_MAX];
  GetCurrentDir( buff, FILENAME_MAX );
  std::string current_working_dir(buff);
  return current_working_dir;
}

int BufferClass::explore(const char *dirname) {
    // return the current length for level 1 array
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
    // level.currentLevel = 1;
    // level.currentSize = 0;
    // level.bufferLocation[level.currentSize] = filename;

    // KeyValuePair keyValueArray[BUFFER_SIZE];
    // currentSize = 0;

}
    // DIR *dir;
    // struct dirent *entry;
    // struct stat info;
    // dir = opendir(dirname);
    // if (!dir) {
    //     return "Directory doesn't exist\n";
    // }
    // while ((entry = readdir(dir)) != NULL) {
    //     if (entry -> d_name[0] != '.') {
    //         string path = string(dirname) + "/" + string(entry -> d_name);
    //         string path_ = path;
    //         const int idx = path.find_last_of("\\/");
    //         const size_t period_idx = path.rfind('.');
    //         if (path.find("lsm_data") == 2) {
                
    //             if (std::string::npos != idx) {
    //                 path_.erase(0, idx + 1);
    //             }
    //             cout << "Entry = " << path << "-----------" << path_.substr(0, path_.size()-4) << endl;
    //             stat((char*)path.c_str(), &info);
    //             if (S_ISDIR(info.st_mode)) {
    //                 explore((char*)path.c_str());
    //             } 
    //         }
    //     }
    // }
    // closedir(dir);