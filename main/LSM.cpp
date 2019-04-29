#include "LSM.h"
using namespace std;

void LSM::driverLeveling(int operation, int key, string value, int targetKey, int lowerBound, int upperBound, int Q, int T) {
    bool flushed = false;
    switch (operation) {
        case 0: {
            cout << "Inserting key " << key << " and value " << value << endl;
            buffer.insert(key, value, false);
            if (buffer.currentSize == BUFFER_SIZE) {
                // flushLevel() would sortMerge before flush, and currentLevel will always be >= 1.
                cout << "FLUSHING! current level: " << currentLevel << ", lsm size: " << LSMLevel.size() << endl;
                int totalPairs = buffer.flushLevel(currentLevel);
                for (int i=0; i<LSMLevel.size(); i++) {
                    if (LSMLevel[i].totalNumberOfPairs > BUFFER_SIZE * pow(SIZE_RATIO, i)) {
                        
                    } else {
                        currentLevel += 1;
                        totalPairs = buffer.flushLevel(currentLevel);
                        string filename = "lsm_data/level_" + to_string(i) + "_file_1.txt";
                        int ranges[] = {buffer.smallest, buffer.largest};
                        levelMetadata newLevel = {currentLevel, filename, *ranges, totalPairs};
                        LSMLevel.push_back(newLevel);
                    }
                }
            }
                break;
        }
        case 1: {
            buffer.insert(key, value, false);
            cout << "updated key " + to_string(key) + " and value" + value << endl;
            if (buffer.currentSize == BUFFER_SIZE) {
                // buffer.flush();
                buffer.currentSize = 0;
                flushed = true;

            }
            if (flushed == true){
                int numOfLevels = LSMLevel.size();
                for (int i = 0; i < numOfLevels; i++){
                    if (LSMLevel[i].totalNumberOfPairs >= (Q * (T^i))){
                        flushLevel(i);
                    } else {
                        break;
                    }
                }
            }
            
            break;
        }
        case 2: {
            buffer.insert(key, "", true);
            cout << "deleted key " + to_string(key) << endl;
            if (buffer.currentSize == BUFFER_SIZE) {
                // buffer.flush();
                buffer.currentSize = 0;
                flushed = true;

            }
            if (flushed == true){
                int numOfLevels = LSMLevel.size();
                for (int i = 0; i < numOfLevels; i++){
                    if (LSMLevel[i].totalNumberOfPairs >= (Q * (T^i))){
                        flushLevel(i);
                    } else {
                        break;
                    }
                }
            }
            
            break;
        }
        case 3: {
            string pointLookup = buffer.searchKeyInBuffer(targetKey);
            if (pointLookup != "") 
                cout << "found key " + to_string(targetKey) + "'s value as " + pointLookup  << endl;
            else if (pointLookupLevel(targetKey) != "") {
                pointLookup = pointLookupLevel(targetKey);
                cout << "found key " + to_string(targetKey) + "'s value as " + pointLookup  << endl;
            } else {
                cout << "key not found"  << endl;
            }
            break;
        }
        case 4: {
            vector<string> rangeLookup = rangeLookupLevel(lowerBound, upperBound);
            if (rangeLookup.size() > 0) {
                cout << "found: ";
                for (int i=0; i< rangeLookup.size(); i++) {
                    cout << rangeLookup[i];
                }
                cout << endl;
            }
            else {
                cout << "key not found"  << endl;
            }
            break;
        }
        default: {
            cout << "requested operation code " << to_string(operation) << " not found" << endl; 
        }
    }
}


string LSM::searchKeyInFile(string filename, int targetKey) {
    ifstream targetFile(filename);
    // it's not openable only when it got deleted?
    if (!targetFile) cerr << "Target file doesn't contain the given key.";
    int key;
    string value;
    bool flag;
    while (targetFile >> key >> value >> flag) {
        if (key == targetKey && !flag) {
            return value;
        }
    }
    targetFile.close();
    
    return value;
}

/**
 * searches the value of the input key in a LSM with leveling
 * 
 * checks all fence pointers to see which level's key range the given key falls into, 
 * then searches the key while traversing the level's file. It's guaranteed to 
 * have the latest value of the target key because we exit out of the function once 
 * found any value while traversing from newest level.
 * 
 * @param[key:required] key to lookup in all levels
 * @return value of the key if key exists, or an error message elsewise
 */



string LSM::pointLookupLevel(int key) {
    for (int levelNumber = 0; levelNumber < LSMLevel.size(); levelNumber++) {
        levelMetadata curLevel = LSMLevel[levelNumber];
        if (key >= curLevel.keyRange[0] and key < curLevel.keyRange[1]) {
            return searchKeyInFile(curLevel.filename, key);
        }
    }

    return "";
}

/**
 * searches the value of the input key in a LSM with tiering
 * 
 * checks all fence pointers to see which tier's key range the given key falls into, 
 * then searches the key while traversing the tier's file. It's guaranteed to 
 * have the latest value of the target key because we exit out of the function once 
 * found any value while traversing from newest level.
 * 
 * @param[key:required] key to lookup in all levels
 * @return value of the key if key exists, or an error message elsewise
 */
string LSM::pointLookupTier(int key) {
    for (int levelNumber = 0; levelNumber < LSMTier.size(); levelNumber++) {
        tierMetadata curLevel = LSMTier[levelNumber];
        for (int tierNumber = 0; tierNumber < curLevel.tierData.size(); tierNumber++) {
            tier curTier = curLevel.tierData[tierNumber];
            if (key >= curTier.keyRange[0] and key < curTier.keyRange[1]) {
                return searchKeyInFile(curTier.filename, key);
            }
        }
    }

    return "";
}

/**
 * searches the values whose keys fall into the given lowerBound and upperBound key range
 * 
 * @param[lowerBoundKey:required] the smaller key whose value will be returned
 * @param[upperBoundKey:required] the largest key whose value will be returned 
 * @return a vector of values in form of string
 */
vector<string> LSM::rangeLookupTier(int lowerBoundKey, int upperBoundKey) {
    // 1. get the potential level's filenames - constant time k
    vector<string> filenames;
    for (int levelNumber = 0; levelNumber < LSMTier.size(); levelNumber++) {
        tierMetadata curLevel = LSMTier[levelNumber];
        for (int tierNumber = 0; tierNumber < curLevel.tierData.size(); tierNumber++) {
            tier curTier = curLevel.tierData[tierNumber];
            if (lowerBoundKey >= curTier.keyRange[0] or upperBoundKey < curTier.keyRange[1]) {
                filenames.push_back(curTier.filename);
            }
        }
    }

    // 2. combine all KeyValuePairs and then sort merge them - O(logN * logN)
    LevelClass lv;
    int count = 0;
    vector<KeyValuePair> tmp;
    while (count != filenames.size()) {
        if (count == 0) {
            vector<KeyValuePair> vec1 = lv.readFile(filenames[count]);
            vector<KeyValuePair> vec2 = lv.readFile(filenames[count + 1]);
            tmp = lv.sortMerge(vec1, vec2);
        }
        else {
            vector<KeyValuePair> vec3 = lv.readFile(filenames[count]);
            tmp = tmp.size() > 0 ? lv.sortMerge(tmp, vec3) : tmp;
        }

        count += 2;
    }

    // 3. search lowerbound - O(logN) + keep appending values to ret until upperBound - O(n)
    vector<string> ret;
    int lowerBoundIndex = searchKey(tmp, lowerBoundKey);
    for (int i=lowerBoundIndex; i<tmp.size(); i++) {
        if (tmp[i].key != upperBoundKey) ret.push_back(tmp[i].value);
    }

    return ret;
}


vector<string> LSM::rangeLookupLevel(int lowerBoundKey, int upperBoundKey) {
    // 1. get the potential level's filenames - constant time k
    vector<string> filenames;
    for (int levelNumber = 0; levelNumber < LSMLevel.size(); levelNumber++) {
        levelMetadata curLevel = LSMLevel[levelNumber];
        if (lowerBoundKey >= curLevel.keyRange[0] or upperBoundKey < curLevel.keyRange[1]) {
            filenames.push_back(curLevel.filename);
        }
    }

    // 2. combine all KeyValuePairs and then sort merge them - O(logN * logN)
    LevelClass lv;
    int count = 0;
    vector<KeyValuePair> tmp;
    while (count != filenames.size()) {
        if (count == 0) {
            vector<KeyValuePair> vec1 = lv.readFile(filenames[count]);
            vector<KeyValuePair> vec2 = lv.readFile(filenames[count + 1]);
            tmp = lv.sortMerge(vec1, vec2);
        }
        else {
            vector<KeyValuePair> vec3 = lv.readFile(filenames[count]);
            tmp = tmp.size() > 0 ? lv.sortMerge(tmp, vec3) : tmp;
        }

        count += 2;
    }

    // 3. search lowerbound - O(logN) + keep appending values to ret until upperBound - O(n)
    vector<string> ret;
    int lowerBoundIndex = searchKey(tmp, lowerBoundKey);
    for (int i=lowerBoundIndex; i<tmp.size(); i++) {
        if (tmp[i].key != upperBoundKey) ret.push_back(tmp[i].value);
    }

    return ret;
}

bool LSM::checkFlushTier(int levelNumber) {
    return (LSMTier[levelNumber].tierData.size() >= SIZE_RATIO - 1) ? true : false;
}

bool LSM::checkFlushLevel(int levelNumber) {
    return (LSMLevel[levelNumber].totalNumberOfPairs >= (pow(SIZE_RATIO, levelNumber) * BUFFER_SIZE)) ? true : false;
}

void LSM::flushLevel(int levelNumber) {
    LevelClass lv;
    // read and remove current level's data/file
    string originalFilename = "lsm_data/level_" + to_string(levelNumber) + "_file_1.txt";
    vector<KeyValuePair> tmp = lv.readFile(originalFilename);
    char *fileToDelete = &originalFilename[0u];
    remove(fileToDelete);
    // sort-merge the flushed data with the next level if there is data
    string newFilename = "lsm_data/level_" + to_string(levelNumber + 1) + "_file_1.txt";
    vector<KeyValuePair> tmp2 = lv.readFile(newFilename);
    if (tmp2.size() > 0) {
        tmp2 = lv.sortMerge(tmp, tmp2);
    }
    // write to the next level's file
    std::ofstream targetFile (newFilename);
    for (int i=0; i < tmp2.size() ; i++) {
        int key = tmp2[i].key;
        string value = tmp2[i].value;
        bool flag = tmp2[i].flag;
        targetFile << key << " " << value << " " << flag << "\n";
    }
    targetFile.close();
}

vector<KeyValuePair> LSM::flushTier(int levelNumber) {
    LevelClass lv;
    lv.generateFilenameList();
    lv.leveling();
    vector<KeyValuePair> tmp(lv.levelArray, lv.levelArray + BUFFER_SIZE * SIZE_RATIO);
    for (int i=0; i < SIZE_RATIO; i++) {
        string originalFilename = lv.filenameList[i];
        char *fileToDelete = &originalFilename[0u];
        remove(fileToDelete);
    }
    // write to the next level's next file
    int nextPage = LSMTier[levelNumber].tierData.size() + 1;
    string newFilename = "lsm_data/level_" + to_string(levelNumber + 1) + "_file_" + to_string(nextPage) + ".txt";
    std::ofstream targetFile (newFilename);
    for (int i=0; i < BUFFER_SIZE ; i++) {
        int key = tmp[i].key;
        string value = tmp[i].value;
        bool flag = tmp[i].flag;
        targetFile << key << " " << value << " " << flag << "\n";
    }
    targetFile.close();

    return tmp;
}



/**
 * prints out the entire LSM-Tree
 * 
 * @param void
 * @return void
 */
void LSM::print_LSM() {
    cout << "LSM data" << endl;
    return;
}

/**
 * @depricated
 * searches a given key with binary search approach
 * 
 * this binary search is advised against fast_upper_bound3 for faster lookup in C++
 * 
 * @param[vector<KeyValuePair> vec:required] a vector of KeyValuePair struct, 
 *   which is sorted before they're written into a sst-like txt file 
 * @param[key:required] the key to search
 * @return the value of a given key in form of string if found, else empty string
 * @see https://academy.realm.io/posts/how-we-beat-cpp-stl-binary-search/
 */
int LSM::searchKey(vector<KeyValuePair> vec, int key) {
    int size = vec.size();
    int low = 0;
    
    while (size > 0) {
        int half = size / 2; // size of first half of vec
        int other_half = size - half; // size of second half of vec
        int curIndex = low + half; // current checking pointer index in the vector
        int other_low = low + other_half; // low index of second half
        KeyValuePair tmp = vec[curIndex];
        int curKey = tmp.key;
        if (curKey == key) return curIndex;
        size = half;
        low = curKey < key ? other_low : low;
    }
    
    return low;
}



void LSM::driverTiering(int operation, int key, string value, int targetKey, int lowerBound, int upperBound, int Q, int T) {
    int bufferSize = buffer.currentSize;
    bool flushed = false;

    // vector<levelMeta> levelMetadata = getMetadata();
    // // int totalLevel = levelMetadata.size();
    // int currentLevel = 1;
    // levelMeta curLevel = levelMetadata[currentLevel];
    // int levelSize = levelMetadata.back().levelNumber;
    //TODO: runtime for print to substruct from overall runtime?
    switch (operation) {
        //TODO: make these all alive with manifest file
                // string newFilename = buffer.flushLevel(currentLSM.size() + 1);
                // int ranges[] = {buffer.smallest, buffer.largest};
                // levelMetadata newPage = {lsm.currentLevel, newFilename, *ranges, 1};
                // newPage.totalNumberOfPairs = buffer.totalNonDup;
                // currentLSM.push_back(newPage);
                // lsm.currentLevel++;
                // buffer.totalNonDup = 0;
                // int levelSize = currentLSM[lsm.currentLevel].totalNumberOfPairs;
                //     cout << currentLSM[i].filename << endl;
                //     if (kv.size() >= (Q * pow(T, lsm.currentLevel))) {
                //         cout << "we should have a new level" << endl;
                //         lsm.currentLevel --;
                //     }
                //     if (currentLSM[i].totalNumberOfPairs >= (Q * (T^i))){
                //         lsm.flushLevel(i);
                //         lsm.currentLevel ++;
                //         cout << "new level: " << lsm.currentLevel << endl;
                //     } else {
                //         cout << "here!" << endl;
                //         break;
                //     }
            //}
            
            //break;
        case 0: {
            buffer.insert(key, value, false);
            bufferSize ++;
            //cout << "inserted key " + to_string(key) + " and value " + value << endl;
            cout <<  " buffer Size " + to_string(bufferSize) << endl;
            cout << to_string(bufferSize == BUFFER_SIZE) << endl;
            if (bufferSize == BUFFER_SIZE) {
                cout<<  " FLUSH Size " + to_string(bufferSize) << endl;
                string newfile = buffer.flushTier(LSMTier.size());
                insertTier(newfile, 1);



                buffer.currentSize = 0;
                int numOfTiers = LSMTier.size();

                for (int i = 0; i < numOfTiers; i++){
                    if (LSMTier[i].totalNumberOfTiers >= T){
                        insertTier(______, i);
                    } else {
                        break;
                    }
                }
            }
            flushed = false;
            
            break;
        }
        case 1: {
            buffer.insert(key, value, false);
            cout << "updated key " + to_string(key) + " and value" + value << endl;
            if (bufferSize >= BUFFER_SIZE) {
                string newfile = buffer.flushTier(LSMTier.size());
                buffer.currentSize = 0;
                flushed = true;
            }
            if (flushed == true){
                int numOfTiers = LSMTier.size();
                for (int i = 0; i < numOfTiers; i++){
                    if (LSMTier[i].totalNumberOfTiers >= T){
                        flushTier(i);
                    } else {
                        break;
                    }
                }
            }
            
            break;
        }
        case 2: {
            buffer.insert(key, "", true);
            cout << "deleted key " + to_string(key) << endl;
            if (bufferSize >= BUFFER_SIZE) {
                string newfile = buffer.flushTier(LSMTier.size() );
                buffer.currentSize = 0;
                flushed = true;
            }
            if (flushed == true){
                int numOfTiers = LSMTier.size();
                for (int i = 0; i < numOfTiers; i++){
                    if (LSMTier[i].totalNumberOfTiers >= T){
                        flushTier(i);
                    } else {
                        break;
                    }
                }
            }
            
            break;
        }
        // case 3: {
        //     string pointLookup = buffer.searchKeyInBuffer(targetKey);
        //     if (pointLookup != "") 
        //         cout << "found key " + to_string(targetKey) + "'s value as " + pointLookup  << endl;
        //     else if (lsm.pointLookupLevel(targetKey) != "") {
        //         pointLookup = lsm.pointLookupLevel(targetKey);
        //         cout << "found key " + to_string(targetKey) + "'s value as " + pointLookup  << endl;
        //     } else {
        //         cout << "key not found"  << endl;
        //     }
        //     break;
        // }
        // case 4: {
        //     vector<string> rangeLookup = lsm.rangeLookupTier(lowerBound, upperBound);
        //     if (rangeLookup.size() > 0) {
        //         cout << "found: ";
        //         for (int i=0; i< rangeLookup.size(); i++) {
        //             cout << rangeLookup[i];
        //         }
        //         cout << endl;
        //     }
        //     else {
        //         cout << "key not found"  << endl;
        //     }
        //     break;
        // }
        default: {
            cout << "requested operation code " << to_string(operation) << " not found" << endl; 
        }
    }
}


void inserTier(string filename, int level){
    std::ifstream infile(filename);
    int key1;
    string value;
    bool flag;

while (infile >> key1 >> value >> flag){
        (operation, key1, value, key1, key1, key2, Q, T);
    }
}




