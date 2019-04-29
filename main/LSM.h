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

struct levelMetadata {
    string filename;
    int keyRange[2];
    int levelNumber;
};

struct tier {
    string filename;
    int keyRange[2];
    int tierNumber;
};

struct tierMetadata {
    int totalTier;
    vector<tier> tierData;
};

class LSM {
    public:
        vector<levelMetadata> LSMLevel;
        vector<tierMetadata> LSMTier;

        string pointLookupLevel(int key);
        vector<string> rangeLookupLevel(int lowerBoundKey, int upperBoundKey);

        string pointLookupTier(int key);
        vector<string> rangeLookupTier(int lowerBoundKey, int upperBoundKey);

        string searchKeyInFile(string filename, int key);
        vector<KeyValuePair> readKVFromFile(string filename);
        string searchKey(vector<KeyValuePair> vec, int key);
        void print_LSM();
};

#endif