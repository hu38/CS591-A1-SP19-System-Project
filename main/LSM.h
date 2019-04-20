#ifndef LSM_H
#define LSM_H
#include <iostream>
using namespace std;

class LSM {
    public:
        int lsm_tree[5];
        void print_LSM();
    // LevelClass levels[LEVELS];
        string get_value(int key);
};

#endif