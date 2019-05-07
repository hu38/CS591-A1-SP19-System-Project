#include "LSM.h"
using namespace std;

int LSM::findLevelToInsert(int Q, int T, int totalPairs) {
    // for each existing levels (potentially a new level) in LSM tree, we check if it can hold a new run
    int i = 0;
    int count = 0;
    while (i <= LSMLevel.size()) {
        levelMetadata tmp = LSMLevel[i];    
        count += tmp.totalNumberOfPairs;
        // cout << "level " << i << " ranges from " << tmp.keyRange[0] << " to " << tmp.keyRange[1] << " with " << tmp.totalNumberOfPairs << " pairs." << endl; 
        tmp.keyRange[0] = 0;
        tmp.keyRange[1] = 0;
        tmp.totalNumberOfPairs = 0;
        LSMLevel[i] = tmp;
        // if the level's total num of pairs from current interation exceeds its limit on max num of pairs
        if ((count + totalPairs) >= (Q * pow(T, i))) {
            // checking if existing levels can handle this new buffer insertion
            if ((i + 1) < LSMLevel.size()) {
                // cout << "UPDATE " << i << endl; 
                i++;
            // if not, we're creating a new level in the end to hold everything
            } else {
                // cout << "CREATE " << i + 1 << endl; 
                levelMetadata newLevel;
                string filename = "lsm_data/level_" + to_string(i + 2) + "_file_1.txt";
                newLevel.levelNumber = i + 1;
                newLevel.filename = filename;
                newLevel.keyRange[0] = 0;
                newLevel.keyRange[1] = 0;
                LSMLevel.push_back(newLevel);
                i++;
            };
        } else return i;
    }

    return i;
}

/**
 * driver function that takes one single instruction from workload in leveling
 * 
 * @param[operation:required] operation code in form of integer from 0 to 5, representing insert, update, delete, 
 *    point lookup, range lookup respectively
 * @param[key] key to insert, update, or delete
 * @param[value] value of a key
 * @param[targetKey] the key point lookup searches for
 * @param[lowerBound] the lowerbound in range lookup's key range
 * @param[upperBound] the upperBound in range lookup's key range
 * @return void
 */
void LSM::driverLeveling(int operation, int key, string value, int targetKey, int lowerBound, int upperBound, int Q, int T) {
    bool flushed = false;
    int totalPairs;
    switch (operation) {
        case 0: {
            buffer.insert(key, value, false, Q);
            // flush buffer data to the first level
            if (buffer.currentSize >= Q) {
                // reset buffer
                buffer.currentSize = 0;
                // - if LSM tree yet has any levels
                if (LSMLevel.size() == 0) {
                    totalPairs = buffer.flushFirstLevel();
                    levelMetadata firstLevel;
                    string filename = "lsm_data/level_" + to_string(1) + "_file_1.txt";
                    firstLevel.totalNumberOfPairs = totalPairs;
                    firstLevel.keyRange[0] = buffer.smallest;
                    firstLevel.keyRange[1] = buffer.largest;
                    firstLevel.levelNumber = 0;
                    firstLevel.filename = filename;
                    LSMLevel.push_back(firstLevel);
                // - if LSM has at least one level
                } else {
                    totalPairs = buffer.flushFirstLevel();
                    LSMLevel[0].totalNumberOfPairs = totalPairs;
                    LSMLevel[0].keyRange[0] = (buffer.smallest < LSMLevel[0].keyRange[0]) ? buffer.smallest : LSMLevel[0].keyRange[0];
                    LSMLevel[0].keyRange[1] = (buffer.largest > LSMLevel[0].keyRange[1]) ? buffer.largest : LSMLevel[0].keyRange[1];
                    int levelToInsert = findLevelToInsert(Q, T, totalPairs);
                    if (levelToInsert + 1 == LSMLevel.size()) 
                        totalPairs = buffer.flushLevels(levelToInsert);
                    else totalPairs = buffer.flushLevel(levelToInsert);
                    LSMLevel[levelToInsert].totalNumberOfPairs = totalPairs;
                    LSMLevel[levelToInsert].keyRange[0] = buffer.smallest;
                    LSMLevel[levelToInsert].keyRange[1] = buffer.largest;
                }
            }
            break;
        }
        case 1: {
            buffer.insert(key, value, true, Q);
            // flush buffer data to the first level
            if (buffer.currentSize >= Q) {
                // reset buffer
                buffer.currentSize = 0;
                // - if LSM tree yet has any levels
                if (LSMLevel.size() == 0) {
                    totalPairs = buffer.flushFirstLevel();
                    levelMetadata firstLevel;
                    string filename = "lsm_data/level_" + to_string(1) + "_file_1.txt";
                    firstLevel.totalNumberOfPairs = totalPairs;
                    firstLevel.keyRange[0] = buffer.smallest;
                    firstLevel.keyRange[1] = buffer.largest;
                    firstLevel.levelNumber = 0;
                    firstLevel.filename = filename;
                    LSMLevel.push_back(firstLevel);
                // - if LSM has at least one level
                } else {
                    totalPairs = buffer.flushFirstLevel();
                    LSMLevel[0].totalNumberOfPairs = totalPairs;
                    LSMLevel[0].keyRange[0] = (buffer.smallest < LSMLevel[0].keyRange[0]) ? buffer.smallest : LSMLevel[0].keyRange[0];
                    LSMLevel[0].keyRange[1] = (buffer.largest > LSMLevel[0].keyRange[1]) ? buffer.largest : LSMLevel[0].keyRange[1];
                    int levelToInsert = findLevelToInsert(Q, T, totalPairs);
                    if (levelToInsert + 1 == LSMLevel.size()) 
                        totalPairs = buffer.flushLevels(levelToInsert);
                    else totalPairs = buffer.flushLevel(levelToInsert);
                    LSMLevel[levelToInsert].totalNumberOfPairs = totalPairs;
                    LSMLevel[levelToInsert].keyRange[0] = buffer.smallest;
                    LSMLevel[levelToInsert].keyRange[1] = buffer.largest;
                }
            }
            break;
        }
        case 2: {
            string pointLookup = buffer.searchKeyInBuffer(targetKey);
            if (pointLookup != "") pointLookupLevel(targetKey);
            //     cout << "found buffer key " << targetKey << "'s value at " << pointLookup << endl;
            // }
            // else if (pointLookupLevel(targetKey) != "") {
            //     pointLookup = pointLookupLevel(targetKey);
            //     cout << "found level key " + to_string(targetKey) + "'s value at " + pointLookup  << endl;
            // } else {
            //     cout << "key " << targetKey << " not found"  << endl;
            // }
            break;
        }
        case 3: {
            // cout << "searching between range " << lowerBound << " and " << upperBound << endl;
            vector<string> rangeLookup = rangeLookupLevel(lowerBound, upperBound);
            // if (rangeLookup.size() > 0) {
            //     //cout << "found: " << rangeLookup.size() << " results" << endl;
            //     for (int i=0; i< rangeLookup.size(); i++) {
            //         cout << rangeLookup[i] << ' ';
            //     }
            //     cout << endl;
            // }
            // else {
            //     cout << "key ranges " << lowerBound << " to "  << upperBound << " doesn't have values."  << endl;
            // }
            break;
        }
    }
}

void LSM::driverTiering(int operation, int key, string value, int targetKey, int lowerBound, int upperBound, int Q, int T) {
    switch (operation) {
        case 0: {
            buffer.insert(key, value, false, Q);
            if (buffer.currentSize >= Q) { 
                string newfile;
                if (LSMTier.empty()){
                    newfile = buffer.flushTier(0);
                }
                else{
                    newfile = buffer.flushTier(LSMTier[0].tierData.size());
                }
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
            buffer.insert(key, value, true, Q);
            if (buffer.currentSize >= Q) {
                string newfile;
                if (LSMTier.empty()){
                    newfile = buffer.flushTier(0);
                }
                else{
                    newfile = buffer.flushTier(LSMTier[0].tierData.size());
                }
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
            if (pointLookup != "") {pointLookupTier(targetKey);}
            //     cout << "found buffer key " + to_string(targetKey) + "'s value as " + pointLookup  << endl;
            // else if (pointLookupTier(targetKey) != "") {
            //     pointLookup = pointLookupTier(targetKey);
            //     cout << "found level key " + to_string(targetKey) + "'s value as " + pointLookup  << endl;
            // } else {
            //     cout << "key " << targetKey << " not found"  << endl;
            // }
            break;
        }
        case 3: {
            //cout << "SEARCHING between range " << lowerBound << " and " << upperBound << endl;
            vector<string> rangeLookup = rangeLookupTier(lowerBound, upperBound);
            // if (rangeLookup.size() > 0) {
            //     cout << "found: " << rangeLookup.size() << " results." << endl;
            //     for (int i=0; i< rangeLookup.size(); i++) {
            //         cout << rangeLookup[i] << " ";
            //     }
            //     cout << endl;
            // }
            // else {
            //     cout << "key not found"  << endl;
            // }
            break;
        }
    }
}

string LSM::searchKeyInFile(string filename, int targetKey) {
    ifstream targetFile(filename);
    // it's not openable only when it got deleted?
    //if (!targetFile) cerr << "Target file doesn't contain the given key.";
    int key;
    string value;
    bool flag;
    while (targetFile >> key >> value >> flag) {
        if ((key == targetKey) == 1 and flag == 0) {
            return value;
        } else continue;
    }
    targetFile.close();
    
    return "";
}

void LSM::insertTier(string filename, int level){

    // If we are inserting into level 1 it means that we have a buffer file
    tier NewTier;
    if (level == 1){
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
        //cout<< "yhrtwgefwdc    vector Size "<<vec.size() << endl;
        int maxK = vec.back().key;
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
            vec2 = sortMerge(vec2, vec);
            vec.clear();


            char *fileToDelete = &fileToRead[0u];
            remove(fileToDelete);

        } 

        if (LSMTier.size() < level){
            
            filename = "lsm_data/level_" + to_string(level) + "_file_" + to_string(0) + ".txt";
        } else {
            filename = "lsm_data/level_" + to_string(level) + "_file_" + to_string(LSMTier[level-1].totalNumberOfTiers) + ".txt";
        }
        int maxK = vec2.back().key;
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

    // cout << "Tier Num    " << LSMTier[level-1].totalNumberOfTiers;
    // cout << "   Max Key" << LSMTier[level-1].tierData.back().maxkey;
    // cout << "   Min Key" << LSMTier[level-1].tierData.back().minkey << endl;

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
            if (searchKeyInFile(curLevel.filename, key) != "") {
                return searchKeyInFile(curLevel.filename, key);
            } else continue;
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
        vector<tier> curLevel = LSMTier[levelNumber].tierData;
        for (int tierNumber = 0; tierNumber < curLevel.size(); tierNumber++) {
            if (key >= curLevel[tierNumber].minkey or key < curLevel[tierNumber].maxkey) {
                if (searchKeyInFile(curLevel[tierNumber].filename, key) != "") {
                    return searchKeyInFile(curLevel[tierNumber].filename, key);
                } else continue;
            } else continue;
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
    lowerBoundKey = (lowerBoundKey < upperBoundKey) ? lowerBoundKey : upperBoundKey;
    upperBoundKey = (lowerBoundKey < upperBoundKey) ? upperBoundKey : lowerBoundKey;
    vector<string> ret;
    // 1. get the potential level's filenames - constant time k
    vector<string> filenames;
    if (LSMTier.size() < 1 or (lowerBoundKey == upperBoundKey == 0)) return ret;
    for (int levelNumber = 0; levelNumber < LSMTier.size(); levelNumber++) {
        vector<tier> curLevel = LSMTier[levelNumber].tierData;
        for (int tierNumber = 0; tierNumber < curLevel.size(); tierNumber++) {
            tier curTier = curLevel[tierNumber];
            if ((lowerBoundKey >= curTier.minkey or upperBoundKey < curTier.maxkey) and (fexists(&curTier.filename[0u]))) {
                filenames.push_back(curTier.filename);
                //cout << "found file " << curTier.filename << endl;
            }
        }
    }

    // 2. combine all KeyValuePairs and then sort merge them - O(logN * logN)
    if (filenames.size() < 1) return ret;
    // cout << "found " << filenames.size() << " files" << endl;
    int count = 0;
    vector<KeyValuePair> tmp;
    do {
        if (count == 0 and filenames.size() == 1) {   
            vector<KeyValuePair> vec1 = buffer.readFile(filenames[count]);
            tmp = vec1;
            break;
        }
        else if (count == 0 and filenames.size() > 1) {
            vector<KeyValuePair> vec1 = buffer.readFile(filenames[count]);
            vector<KeyValuePair> vec2 = buffer.readFile(filenames[count + 1]);
            tmp = buffer.sortMerge(vec1, vec2);
        }
        else {
            vector<KeyValuePair> vec3 = buffer.readFile(filenames[count]);
            tmp = vec3.size() > 0 ? buffer.sortMerge(tmp, vec3) : tmp;
        }
        count += 1;
    } while (count < filenames.size());

    // 3. search lowerbound - O(logN) + keep appending values to ret until upperBound - O(n)
    if (tmp.size() < 1) return ret;
    int lowerBoundIndex = searchKey(tmp, lowerBoundKey);
    int i=lowerBoundIndex;
    while (tmp[i].key <= upperBoundKey) {
        ret.push_back(tmp[i].value);
        i++;
    }

    return ret;
}


vector<string> LSM::rangeLookupLevel(int lowerBoundKey, int upperBoundKey) {
    lowerBoundKey = (lowerBoundKey < upperBoundKey) ? lowerBoundKey : upperBoundKey;
    upperBoundKey = (lowerBoundKey < upperBoundKey) ? upperBoundKey : lowerBoundKey;
    vector<string> ret;
    // 1. get the potential level's filenames - constant time k
    if (LSMLevel.size() < 1 or (lowerBoundKey == upperBoundKey == 0)) return ret;
    vector<string> filenames;
    for (int levelNumber = 0; levelNumber < LSMLevel.size(); levelNumber++) {
        levelMetadata curLevel = LSMLevel[levelNumber];
        if (lowerBoundKey >= curLevel.keyRange[0] or upperBoundKey < curLevel.keyRange[1]) {
            if (fexists(&curLevel.filename[0u])) {
                filenames.push_back(curLevel.filename);
            }
        }
    }

    // 2. combine all KeyValuePairs and then sort merge them - O(logN * logN)
    if (filenames.size() < 1) return ret;
    int count = 0;
    vector<KeyValuePair> tmp;
    do {
        if (count == 0 and filenames.size() == 1) {   
            vector<KeyValuePair> vec1 = buffer.readFile(filenames[count]);
            tmp = vec1;
            break;
        }
        else if (count == 0 and filenames.size() > 1) {
            vector<KeyValuePair> vec1 = buffer.readFile(filenames[count]);
            vector<KeyValuePair> vec2 = buffer.readFile(filenames[count + 1]);
            tmp = buffer.sortMerge(vec1, vec2);
        }
        else {
            vector<KeyValuePair> vec3 = buffer.readFile(filenames[count]);
            tmp = vec3.size() > 0 ? buffer.sortMerge(tmp, vec3) : tmp;
        }
        count += 1;
    } while (count < filenames.size());

    // 3. search lowerbound - O(logN) + keep appending values to ret until upperBound - O(n)
    if (tmp.size() < 1) return ret;
    int lowerBoundIndex = searchKey(tmp, lowerBoundKey);
    int i=lowerBoundIndex;
    while (tmp[i].key != upperBoundKey) {
        ret.push_back(tmp[i].value);
        i++;
    };

    return ret;
}

/**
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
