#include "LSM.cpp"
#include "BufferClass.cpp"
using namespace std;

int main() {
    // LSM lsm;
    // lsm.print_LSM();
	BufferClass BC;
    BC.currentSize = 0;
	BC.printBC();
    BC.insert(3, "3");
    BC.printBC();
    BC.insert(1, "1");
    BC.printBC();
    BC.insert(6, "6");
    BC.printBC();
    BC.insert(1, "9000");
    BC.printBC();
    BC.insert(2, "2");
    // BC.printBC();
    // BC.insert(4, "4");
    // BC.insert(4, "023132");
    BC.printBC();
    // BC.flush();
    return 0;
}