#ifndef LSM_H
#define LSM_H
#include <iostream>
#include "LevelClass.h"
using namespace std;


struct levelArray {
    int levelNumber;
    int keyRange[2]; // note: [lowerBound, upperBound)
    KeyValuePair pairs[SIZE_RATIO * BUFFER_SIZE];
};


struct levelMeta {
    int levelNumber;
    vector<int> lowerBound;
    vector<int> upperBound;

    //TODO: figure out constructor to avoid copies: levelNumber(lvNum), lowerBound(low), upperBound(upper) {};

    // levelMeta(const levelMeta& meta): 
    //     levelNumber(meta.levelNumber), 
    //     lowerBound(meta.lowerBound), 
    //     upperBound(meta.upperBound) {};
    
    // levelMeta(const int& levelNumber, const vector<int>& lowerBound[BUFFER_SIZE], const vector<int>& upperBound): 
    //     levelNumber(levelNumber), 
    //     lowerBound(lowerBound), 
    //     upperBound(upperBound) {};
};

class LSM {
    public:
        int lsm_tree[5];
        int totalLevel;
        levelArray allLevel[10];

        void insertValue(int key, string value);
        void updateValue(int key, string value);
        void deleteValue(int key);
        string pointLookup(int key);
        vector<string> rangeLookup(int lowerBoundKey, int upperBoundKey);
        
        string searchKey(vector<KeyValuePair> vec, int key);
        void print_LSM();
};

#endif