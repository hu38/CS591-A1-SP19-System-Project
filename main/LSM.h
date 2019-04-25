#ifndef LSM_H
#define LSM_H
#include <iostream>
#include "LevelClass.h"
using namespace std;


struct levelArray {
    int levelNumber;
    int keyRange[2]; // note: [lowerBound, upperBound)
    KeyValuePair levelArray[SIZE_RATIO * BUFFER_SIZE];
};

class LSM {
    public:
        int lsm_tree[5];
        int totalLevel;
        levelArray allLevel[SIZE_RATIO * BUFFER_SIZE];

        void insertValue(int key, string value);
        void updateValue(int key, string value);
        void deleteValue(int key);
        string pointLookup(int key);
        vector<string> rangeLookup(int lowerBoundKey, int upperBoundKey);
        
        void print_LSM();
};

#endif