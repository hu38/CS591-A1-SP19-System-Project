#ifndef LSM_H
#define LSM_H
#include "BufferClass.h"
using namespace std;

struct tier {
    string filename;
    int maxkey;
    int minkey;
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
        
        void driverTiering(int operation, int key = 0, string value = "", int targetKey = 0, int lowerBound = 0, int upperBound = 0, int Q = 0, int T= 0);
        void driverLeveling(int operation, int key, string value, int targetKey, int lowerBound, int upperBound, int Q, int T);
        
        string pointLookupLevel(int key);
        vector<string> rangeLookupLevel(int lowerBoundKey, int upperBoundKey);
        string pointLookupTier(int key);
        vector<string> rangeLookupTier(int lowerBoundKey, int upperBoundKey);
        
        string searchKeyInFile(string filename, int key);
        int searchKey(vector<KeyValuePair> vec, int key);
        void insertTier(string filename, int level);
        vector<KeyValuePair> sortMerge(vector<KeyValuePair> array1, vector<KeyValuePair> array2);
};

#endif