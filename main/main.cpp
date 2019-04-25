#include "BufferClass.cpp"
#include "LevelClass.cpp"
#include "LSM.cpp"
using namespace std;
#include <typeinfo>

int main() {
    // LSM lsm;
    // lsm.print_LSM();
	// BufferClass BC;
    // BC.currentSize = 0;
    // BC.insert(3, "3");
    // BC.insert(1, "1");
    // BC.insert(6, "6");
    // BC.insert(1, "9000");
    // BC.insert(2, "2");
    // BC.insert(4, "4");
    // BC.insert(5, "5");
    // BC.insert(7, "7");
    // BC.printBC();
    LevelClass lv;
    lv.currentLevel = 1;
    lv.generateFilenameList();
    lv.combineLevelArrays();
    lv.printLV();
    return 1;
}

    // char command[50] = "cd lsm_data && ls -l";
    // system(command);
