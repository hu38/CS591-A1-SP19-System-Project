#include<iostream>
#include "LSM.cpp"
#include "KeyValueClass.cpp"
using namespace std;

int main() {
    LSM lsm;
    lsm.print_LSM();
    KeyValueClass kv;
    kv.getData();
    return 0;
}
