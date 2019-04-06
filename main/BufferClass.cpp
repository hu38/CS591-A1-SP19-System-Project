#include "BufferClass.h"
using namespace std;


/*BufferClass BufferClass::sort(BufferClass data) {
    // sort based on key of operations
    SortedBufferClass = library.sort(data) // std::sort() is a sort library
    return(SortedBufferClass)
}*/

/*void BufferClass::flush(BufferClass data, LevelClass level) {
    level.insertBuffer(data);
    data.KeyValueParis = {};
    data.currentSize = 0;
    data.keyRange = {};
    data.time_t = null;
    data.save()
}*/

/*void BufferClass::append(KeyValueClass data, BufferClass bc) {
    current = bc.getCurrentSize();
    bc.keyValuePairs[current] = data;
    bc.currentSize ++;
}*/

int BufferClass::getCurrentSize() {
    return(currentSize);
}

void BufferClass::printBC() {
    cout << "AHOI" << endl;
    return;
}
