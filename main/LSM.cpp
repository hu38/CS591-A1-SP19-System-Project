#include "LSM.h"
using namespace std;

void LSM::insertValue(int key, string value) {
    BufferClass buffer;
    buffer.insert(key, value);
}

void LSM::updateValue(int key, string value) {
    BufferClass buffer;
    buffer.insert(key, value);
}

void LSM::deleteValue(int key) {
    BufferClass buffer;
    buffer.insert(key, "DELETED");
}

string LSM::pointLookup(int key) {
    BufferClass buffer;
    if (buffer.getData(key) != "") {
        return buffer.getData(key);
    } else {
        for (int levelNumber = 0; levelNumber < totalLevel; levelNumber++) {
            if (key >= allLevel[levelNumber].keyRange[0] and key < allLevel[levelNumber].keyRange[1]) {
                for (int curBuffer = 0; curBuffer < BUFFER_SIZE; curBuffer++) {
                    if (key == allLevel[levelNumber].levelArray[curBuffer].key) {
                        return allLevel[levelNumber].levelArray[curBuffer].value; 
                    }
                }
            }
        }
    }
    return "";
}

vector<string> LSM::rangeLookup(int lowerBoundKey, int upperBoundKey) {
    vector<string> ret;
    for (int i = lowerBoundKey; i < upperBoundKey; i++) {
        string tmp = pointLookup(lowerBoundKey);
        if (tmp != "") {
            ret.insert(ret.begin(), tmp);
        }
    }
    return ret;
}

void LSM::print_LSM() {
    cout << "LSM data" << endl;
    return;
}



