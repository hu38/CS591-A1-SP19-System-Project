#include<iostream>
#include "LSM.cpp"
#include "BufferClass.cpp"
using namespace std;

int main() {
    // LSM lsm;
    // lsm.print_LSM();
	BufferClass BC;
	BC.currentSize = 1;
	// int x = BC.getCurrentSize();  
    BC.setData("key1", "value1");
    cout << BC.getData("key1") << endl;
    return 0;
}
