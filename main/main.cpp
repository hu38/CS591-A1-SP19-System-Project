#include<iostream>
#include "LSM.cpp"
#include "BufferClass.cpp"
using namespace std;

int main() {
    // LSM lsm;
    // lsm.print_LSM();
	BufferClass BC;
	BC.currentSize = 0;
	// int x = BC.getCurrentSize();
    BC.insert(3, "3");
    BC.insert(2, "9000");
    BC.insert(1, "1");
    BC.insert(2, "2");
    BC.insert(4, "4");
    BC.sortBC();
    BC.printBC();
    
    
  
    // cout << BC.getData(1) << endl;
    // Json::Value fromScratch;
    // root["buffer"] = BC.keyValueArray;
    // e.push_back(std::make_pair(json::value(0), json::value(false)));
    // e.push_back(std::make_pair(json::value(1), json::value::string(U("hello"))));
    // json::value arr(e)
    return 0;
}
