#include "LSM.h"
using namespace std;

/**
 * searches the value of the input key
 * 
 * checks all fence pointers to see which block/tier's key range the given key falls into, 
 * then invokes a binary search in that tier to retrieve the according value
 * 
 * @param[key:required] key to lookup in all levels
 * @return value of the key if key exists, or an error message elsewise
 */
string LSM::pointLookup(int key) {
    for (int levelNumber = 0; levelNumber < totalLevel; levelNumber++) {
        //TODO: maybe in manifest file?
        levelArray curLevel = allLevel[levelNumber];
        if (key >= curLevel.keyRange[0] and key < curLevel.keyRange[1]) {
            vector<KeyValuePair> tmp (curLevel.pairs, curLevel.pairs + BUFFER_SIZE);
            return searchKey(tmp, key);
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
vector<string> LSM::rangeLookup(int lowerBoundKey, int upperBoundKey) {
    vector<string> ret;
    for (int i = lowerBoundKey; i < upperBoundKey; i++) {
        string tmp = pointLookup(lowerBoundKey);
        if (tmp != "") {
            ret.insert(ret.begin(), tmp);
        }
    }
    return ret;
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
string LSM::searchKey(vector<KeyValuePair> vec, int key) {
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
