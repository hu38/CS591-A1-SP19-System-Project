#ifndef LSM_H
#define LSM_H
#include <cmath>
#include <cstdio>
#include "LevelClass.h"
using namespace std;

struct tier {
    string filename;
    int keyRange[2];
    int tierNumber; // number of sorted runs
};

struct tierMetadata {
    int levelNumber; // the level height is LSM tree
    vector<tier> tierData;
    int totalNumberOfTiers;
};

struct levelMetadata {
    int levelNumber;
    string filename;
    int keyRange[2];
    int totalNumberOfPairs;
};

class LSM {
    public:
        vector<levelMetadata> LSMLevel;
        vector<tierMetadata> LSMTier;
        int currentLevel;
        BufferClass buffer;


        void inserTier(string filename, int level);
        void driverTiering(int operation, int key = 0, string value = "", int targetKey = 0, int lowerBound = 0, int upperBound = 0, int Q = 0, int T= 0);

        string pointLookupLevel(int key);
        vector<string> rangeLookupLevel(int lowerBoundKey, int upperBoundKey);
        string pointLookupTier(int key);
        vector<string> rangeLookupTier(int lowerBoundKey, int upperBoundKey);
        bool checkFlushLevel(int levelNumber);
        bool checkFlushTier(int levelNumber);
        void flushLevel(int currentLevel);
        vector<KeyValuePair> flushTier(int currentLevel);
        void driverLeveling(int operation, int key, string value, int targetKey, int lowerBound, int upperBound, int Q, int T);

        string searchKeyInFile(string filename, int key);
        vector<KeyValuePair> readKVFromFile(string filename);
        int searchKey(vector<KeyValuePair> vec, int key);
        void print_LSM();
};

#endif