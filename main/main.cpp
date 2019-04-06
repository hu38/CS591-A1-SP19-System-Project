#include<iostream>
#include "LSM.h"
#include "KeyValueClass.h"
using namespace std;

int main() {
    LSM lsm;
    lsm.print_LSM();
    KeyValueClass kv;
    kv.getData(3);
    return 0;
}
