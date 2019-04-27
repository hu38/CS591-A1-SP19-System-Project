#include "BufferClass.cpp"
#include "LevelClass.cpp"
#include "LSM.cpp"
using namespace std;
#include <typeinfo>

/**
 * all comments are following https://developer.lsst.io/cpp/api-docs.html standard
 */


/**
 * driver function that takes one single instruction from workload
 * 
 * @param[operation:required] operation code in form of integer from 0 to 5, representing insert, update, delete, 
 *    point lookup, range lookup respectively
 * @param[key] key to insert, update, or delete
 * @param[value] value of a key
 * @param[targetKey] the key point lookup searches for
 * @param[lowerBound] the lowerbound in range lookup's key range
 * @param[upperBound] the upperBound in range lookup's key range
 * @return void
 */
void driver(int operation, int key = 0, string value = "", int targetKey = 0, int lowerBound = 0, int upperBound = 0) {
    BufferClass buffer;
    LevelClass level;
    LSM lsm;
    //TODO: runtime for print to substruct from overall runtime?
    switch (operation) {
        //TODO: make these all alive with manifest file
        case 0: {
            buffer.insert(key, value, false);
            cout << "inserted key " + to_string(key) + " and value" + value << endl;
            break;
        }
        case 1: {
            buffer.insert(key, value, false);
            cout << "updated key " + to_string(key) + " with value" + value << endl;
            break;
        }
        case 2: {
            buffer.insert(key, NULL, true);
            cout << "deleted key " + to_string(key) << endl;
            break;
        }
        case 3: {
            string pointLookup = buffer.searchKeyInBuffer(targetKey);
            if (pointLookup != "") 
                cout << "found key " + to_string(targetKey) + "'s value as " + pointLookup  << endl;
            else if (lsm.pointLookup(targetKey) != "") {
                pointLookup = lsm.pointLookup(targetKey);
                cout << "found key " + to_string(targetKey) + "'s value as " + pointLookup  << endl;
            } else {
                cout << "key not found"  << endl;
            }
            break;
        }
        case 4: {
            vector<string> rangeLookup = lsm.rangeLookup(lowerBound, upperBound);
            if (rangeLookup.size() > 0) {
                cout << "found: ";
                for (int i=0; i< rangeLookup.size(); i++) {
                    cout << rangeLookup[i];
                }
                cout << endl;
            }
            else {
                cout << "key not found"  << endl;
            }
            break;
        }
        default: {
            cout << "requested operation code " << to_string(operation) << " not found" << endl; 
        }
    }
}

int main() {
    // 0. preparation
    // 0.1 create a folder for LSM data with read and write privileges
    // if folder exists, recreating won't swipe off files inside and start from scratch
    mkdir("lsm_data/", S_IRWXU); 
    // 0.2 create a manifest file to log LSM execuations
    FILE *manifest;
    manifest = fopen ("lsm_data/lsm.meta", "w"); 
    if (manifest == NULL) {
        fprintf(stderr, "\nError opend file\n"); 
        exit (1); 
    }
    // KeyValuePair kv[2];
    // kv[0] = (KeyValuePair) {1, "1", true}; 
    // we expect a max of 10 levels in lsm
    vector<levelMeta> meta;
    meta.reserve(10);
    // vector<int> r;
    // r.push_back(1);
    levelMeta m = {1, {{1, 2}}, {{3, 4}}};
    meta[0] = m;
    // write struct to file 
    fwrite (&meta, sizeof(meta), 10, manifest); 
    cout << sizeof(meta) << endl;
    if(fwrite != 0)
        printf("contents to file written successfully !\n"); 
    else
        printf("error writing file !\n"); 
    // close file 
    fclose (manifest); 


    // LSM lsm;
    // lsm.print_LSM();
	// BufferClass BC;
    // BC.currentSize = 0;
    // BC.insert(3, "3", false);
    // BC.insert(1, "1", false);
    // BC.insert(6, "6", false);
    // BC.insert(2, "2", false);
    // BC.insert(4, "4", false);
    // BC.insert(5, "5", false);
    // BC.insert(1, NULL, true); //FIXME: bc doesn't handle delete to null
    // BC.insert(7, "7", false);
    // BC.printBC();
    // LevelClass lv;
    // lv.currentLevel = 1;
    // lv.generateFilenameList();
    // lv.combineLevelArrays();
    // lv.printLV();
    // vector<KeyValuePair> vec = lv.readFile("lsm_data/level_1_file_1.txt");
    // LSM lsm;
    FILE *infile; 
    levelMeta input;
    infile = fopen ("lsm_data/lsm.meta", "r"); 
    if (infile == NULL) { 
        fprintf(stderr, "\nError opening file\n"); 
        exit (1); 
    }  
    // read file contents till end of file 
    while(fread (&input, sizeof(input), 1, infile)) {
        cout << "level: " << to_string(input.levelNumber) << endl;
        std::ostringstream oss;
        vector<int> vec = input.lowerBound;
        if (!vec.empty())
        {
            // Convert all but the last element to avoid a trailing ","
            std::copy(vec.begin(), vec.end()-1,
                std::ostream_iterator<int>(oss, ","));

            // Now add the last element with no delimiter
            oss << vec.back();
        }
        std::cout << oss.str() << std::endl;
        // string str(input.lowerBound, input.lowerBound);
        // cout << "lower bound: " << str << endl;
    }
    // close file 
    fclose (infile); 
    return 1;
}

    // char command[50] = "cd lsm_data && ls -l";
    // system(command);

    /////////////////////////FILE EXAMPLE////////////////////////

    
    // ////////////////////////////////////////////////////////////////////////////////////
    
