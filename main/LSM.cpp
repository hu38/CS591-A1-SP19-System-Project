#include "LSM.h"
using namespace std;


// void LSM::insertTier(string filename, int level){

//     // If we are inserting into level 1 it means that we have a buffer file
//     tier NewTier;
//     if (level == 1){
//         cout << "P" << endl;
//         std::ifstream infile(filename);
//         int key1;
//         string value;
//         bool flag;

//         vector<KeyValuePair> vec;
//         KeyValuePair NewPair;

//         while (infile >> key1 >> value >> flag){
//             NewPair = (KeyValuePair) {key1, value, flag};
//             vec.insert(vec.end(), NewPair);
//         }
//         int maxK = (int) vec[vec.size()].key;
//         int minK = vec[0].key;

        
//         NewTier = (tier) {filename, maxK, minK, 0};
//     } else{
//         cout << "h" << endl;
//         vector<KeyValuePair> vec;
//         KeyValuePair NewPair;

//         for (int j =0; j < LSMTier[level-1].totalNumberOfTiers; j++){
//             string fileToRead = LSMTier[level-1].tierData[j].filename;
//             std::ifstream infile(fileToRead);

//             int key1;
//             string value;
//             bool flag;
//             while (infile >> key1 >> value >> flag){
//                 NewPair = (KeyValuePair) {key1, value, flag};
//                 vec.insert(vec.end(), NewPair);
//             }


//         }
//         if (LSMTier[level].tierData.empty()){
//             filename = "level_" + to_string(level) + "_file_" + to_string(0) + ".txt";
//         } else {
//             filename = "level_" + to_string(level) + "_file_" + to_string(LSMTier[level].totalNumberOfTiers) + ".txt";
//         }
//         int maxK = (int) vec[vec.size()].key;
//         int minK = vec[0].key;

//         NewTier = (tier) {filename, maxK, minK, 0};
        
//     }

    

// ////////////////////////

//     // check if there is a vector in the level otherwise start a new one

//     cout << "a1" << endl;
//     if (LSMTier.empty() || level > LSMTier.size()){
//         cout << "a2" << endl;
//         vector<tier> newtierData;
//         newtierData.push_back(NewTier);
        
//         tierMetadata newtierMetadata = (tierMetadata) {level, newtierData, 1};

//         LSMTier.push_back(newtierMetadata);
//     } else {
//         cout << "a3" << endl;
//         LSMTier[level-1].tierData.push_back(NewTier);
//         cout << "a6" << endl;
//         LSMTier[level-1].totalNumberOfTiers ++;
//     }

//     //cout << "TIERS " << LSMTier[level-1].totalNumberOfTiers << endl;
//     //cout << "Level " << LSMTier[level-1].levelNumber << endl;
//     cout << "Files " << LSMTier[level-1].tierData[0].filename << LSMTier[level-1].tierData[1].filename << endl;

    

//     //LSMTier[level].totalNumberOfTiers = LSMTier[level].tierData.size();

//     //cout <<  LSMTier[level].totalNumberOfTiers << endl;

// }

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
            if (key >= curTier.minkey and key < curTier.maxkey) {
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
            if (lowerBoundKey >= curTier.minkey or upperBoundKey < curTier.maxkey) {
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

void LSM::insertTier(string filename, int level){
    cout<< level << endl;

    // If we are inserting into level 1 it means that we have a buffer file
    tier NewTier;
    if (level == 1){
        cout << "P" << endl;
        std::ifstream infile(filename);
        int key1;
        string value;
        bool flag;

        vector<KeyValuePair> vec;
        KeyValuePair NewPair;

        while (infile >> key1 >> value >> flag){
            NewPair = (KeyValuePair) {key1, value, flag};
            vec.insert(vec.end(), NewPair);
        }
        int maxK = (int) vec[vec.size()].key;
        int minK = vec[0].key;

        
        NewTier = (tier) {filename, maxK, minK, 0};
    } else {
        
        vector<KeyValuePair> vec;
        vector<KeyValuePair> vec2;
        KeyValuePair NewPair;
        
        for (int j =0; j < LSMTier[level-2].totalNumberOfTiers; j++){
            string fileToRead = LSMTier[level-2].tierData[j].filename;
            std::ifstream infile(fileToRead);

            int key1;
            string value;
            bool flag;
            while (infile >> key1 >> value >> flag){
                NewPair = (KeyValuePair) {key1, value, flag};
                vec.insert(vec.end(), NewPair);
            }
            //cout<< "Before " << endl;
            // for (int p =0; p < vec.size(); p++){
            //     cout << vec[p].key << " ";
            // }
            cout<< "BETWEEN " << endl;


            cout<< "After " << endl;

            vec2 = sortMerge(vec2, vec);
            vec.clear();


            char *fileToDelete = &fileToRead[0u];
               remove(fileToDelete);

        } 

        cout<< "DFGHJKLKJHHHHHH " << LSMTier.size() << "  " << level << endl;

        if (LSMTier.size() < level){
            filename = "lsm_data/level_" + to_string(level) + "_file_" + to_string(0) + ".txt";
        } else {
            filename = "lsm_data/level_" + to_string(level) + "_file_" + to_string(LSMTier[level-1].totalNumberOfTiers) + ".txt";
        }
        int maxK = (int) vec2[vec2.size()].key;
        int minK = vec2[0].key;

        std::ofstream newfile (filename);
        for (int i=0; i < vec2.size() ; i++) {
            int key = vec2[i].key;
            string value = vec2[i].value;
            bool flag = vec2[i].flag;
            newfile << key << " " << value << " " << flag << "\n";
        }
        newfile.close();

        NewTier = (tier) {filename, maxK, minK, 0};
    }
    // check if there is a vector in the level otherwise start a new level
    if (LSMTier.empty() || level > LSMTier.size()){
        vector<tier> newtierData;
        newtierData.push_back(NewTier);
        
        tierMetadata newtierMetadata = (tierMetadata) {level, newtierData, 1};

        LSMTier.push_back(newtierMetadata);
    } else {
        LSMTier[level-1].tierData.push_back(NewTier);
        LSMTier[level-1].totalNumberOfTiers ++;
    }

    //cout << "SIZE OF LSM"<< LSMTier.size() << endl;
    cout << "TIERS " << LSMTier[level-1].totalNumberOfTiers << endl;
    cout << "Level " << LSMTier[level-1].levelNumber << endl;
    cout << "Files " << LSMTier[level-1].tierData.back().filename << endl;
    //cout << "Files " << LSMTier[0].tierData[0].filename << LSMTier[level-1].tierData[0].filename << endl;

}


void LSM::driverTiering(int operation, int key, string value, int targetKey, int lowerBound, int upperBound, int Q, int T) {
    int bufferSize = buffer.currentSize;
    bool flushed = false;

    switch (operation) {

        case 0: {
            buffer.insert(key, value, false);
            //cout << "inserted key " + to_string(key) + " and value " + value << endl;
            //cout <<  " buffer Size " + to_string(bufferSize) << endl;
            //cout << to_string(bufferSize == BUFFER_SIZE) << endl;
            if (buffer.currentSize == Q) {
                //cout<<  " FLUSH Size " + to_string(bufferSize) << endl;
                string newfile;
                if (LSMTier.empty()){
                    //cout << " It's emtpy "<< endl;
                    newfile = buffer.flushTier(0);
                }
                else{
                    //out << " It's not emtpy "<< endl;
                    newfile = buffer.flushTier(LSMTier[0].tierData.size());
                }
                //cout<<  " File Name is " + newfile << endl;
                insertTier(newfile, 1);
                buffer.currentSize = 0;
                
                int numOfLevels = LSMTier.size();

                for (int i = 1; i <= numOfLevels; i++){
                    if (LSMTier[i-1].totalNumberOfTiers >= T){
                        insertTier(newfile, i+1);

                        LSMTier[i-1].totalNumberOfTiers = 0;
                        vector<tier> newtierData;
                        LSMTier[i-1].tierData = newtierData;


                    } else {
                        break;
                    }
                }
            }
            break;
        }

        case 1: {
            buffer.insert(key, "", true);
            //cout << "inserted key " + to_string(key) + " and value " + value << endl;
            //cout <<  " buffer Size " + to_string(bufferSize) << endl;
            //cout << to_string(bufferSize == BUFFER_SIZE) << endl;
            if (buffer.currentSize == Q) {
                //cout<<  " FLUSH Size " + to_string(bufferSize) << endl;
                string newfile;
                if (LSMTier.empty()){
                    //cout << " It's emtpy "<< endl;
                    newfile = buffer.flushTier(0);
                }
                else{
                    //out << " It's not emtpy "<< endl;
                    newfile = buffer.flushTier(LSMTier[0].tierData.size());
                }
                //cout<<  " File Name is " + newfile << endl;
                insertTier(newfile, 1);
                buffer.currentSize = 0;
                
                int numOfLevels = LSMTier.size();

                for (int i = 1; i <= numOfLevels; i++){
                    if (LSMTier[i-1].totalNumberOfTiers >= T){
                        insertTier(newfile, i+1);

                        LSMTier[i-1].totalNumberOfTiers = 0;
                        vector<tier> newtierData;
                        LSMTier[i-1].tierData = newtierData;


                    } else {
                        break;
                    }
                }
            }
            break;
        }
        case 2: {
            string pointLookup = buffer.searchKeyInBuffer(targetKey);
            if (pointLookup != "") 
                cout << "found key " + to_string(targetKey) + "'s value as " + pointLookup  << endl;
            else if (pointLookupTier(targetKey) != "") {
                pointLookup = pointLookupTier(targetKey);
                cout << "found key " + to_string(targetKey) + "'s value as " + pointLookup  << endl;
            } else {
                cout << "key not found"  << endl;
            }
            break;
        }
        case 3: {
            vector<string> rangeLookup = rangeLookupTier(lowerBound, upperBound);
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

vector<KeyValuePair> LSM::sortMerge(vector<KeyValuePair> array1, vector<KeyValuePair> array2) {
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







