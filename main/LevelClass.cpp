#include <iostream>
#include <array>

// Append: called from a BufferClass object, adds a keyValue object to end of array

void LevelClass::sortMerge()
{

	BufferClass answer = new BufferClass()
    int i = 0, j = 0, k = 0;

    a = bf[0]
    b = bf[1]

    while (i < a.currentSize && j < b.currentSize){
    	if a[i].key < b[j].key{
    		answer.append(a[i])
    	}
    	else if a[i].key > b[j].key{

    		answer.append(b[j])
    	}
    	else{
    		answer.append(b[j]) // for now, we can look into it later
    	}
    }

    while (i < a.currentSize){
    	answer.append(a[i])
    } 

    while (j < b.currentSize) {   
        answer.append(b[i])
    }
    // Call Append into a new level with the class
   	bf = bf[2:]
    return(MergedSortedLevelClass)

}