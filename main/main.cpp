// #include "BufferClass.cpp"
#include "LevelClass.cpp"
#include "LSM.cpp"
using namespace std;
#include <typeinfo>

int main() {
    // LSM lsm;
    // lsm.print_LSM();
	// BufferClass BC;
    // BC.currentSize = 0;
    // BC.insertKV(3, "3");
    // BC.insertKV(1, "1");
    // BC.insertKV(6, "6");
    // BC.insertKV(1, "9000");
    // BC.insertKV(2, "2");
    // BC.insertKV(4, "4");
    // BC.updateKV(4, "023132");
    // BC.insertKV(5, "5");
    // BC.insertKV(7, "7");
    // BC.printBC();
    LevelClass lv;
    lv.readFile("lsm_data/level_1_file_1.txt");
    // cout << typeid(bf).name() << endl;
    lv.printLV();
    return 1;
}

    // char command[50] = "cd lsm_data && ls -l";
    // system(command);