#include<iostream>
#include "LSM.cpp"
#include "KeyValueClass.cpp"
#include "BufferClass.cpp"
using namespace std;

int main() {
    LSM lsm;
    lsm.print_LSM();
    KeyValueClass kv;
    kv.getData(3);

	BufferClass BC;
	BC.printBC();  
	BC.currentSize = 5;
	int x = BC.getCurrentSize();  
	cout << x << endl;
    return 0;
}
