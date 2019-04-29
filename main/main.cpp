#include "BufferClass.cpp"
#include "LevelClass.cpp"
#include "LSM.cpp"
using namespace std;
#include <typeinfo>
#include <cstdlib>

void buildWorkload(string wokload_kind, int total){
    int probabilities[20] = {0,0,0,0,1,1,2,2,3,3,3,3,4,4,4,4};

    if (wokload_kind == "Gral"){
        probabilities[0] = 0;
    } else if (wokload_kind == "Insert"){
        probabilities[5] = 0;
        probabilities[7] = 0;
        probabilities[9] = 0;
        probabilities[13] = 0;
        probabilities[15] = 0;
    } else if (wokload_kind == "Update/Del"){
        probabilities[0] = 1;
        probabilities[9] = 2;
        probabilities[13] = 1;
        probabilities[15] = 2;
    } else if (wokload_kind == "PointR"){
        probabilities[0] = 3;
        probabilities[4] = 3;
        probabilities[6] = 3;
        probabilities[13] = 3;
        probabilities[15] = 3;
    } else {
        probabilities[0] = 4;
        probabilities[1] = 4;
        probabilities[7] = 4;
        probabilities[9] = 4;
        probabilities[11] = 4;
    }


    // Now that the probabilities are specified, generate the workload

    srand(time(NULL));
    int i;
    //OperationKeyValueTrio workload[10*total];
    //int workloadSize = 0;
    std::ofstream bufferFile ("workload.txt");
    for (i = 0; i < total; i++){

        int randIndex = rand() % 20;
        int operation = probabilities[randIndex];
        int key = rand() % (total/2);
        string value = std::to_string(rand() % 1000);
        if (operation == 4){
            int key2 = rand() % total/2;
            if (key > key2){
                bufferFile << operation << " " << key2 << " " << key << " " << value << "\n";
            } else {
                bufferFile << operation << " " << key << " " << key2 << " " << value << "\n";
            }
        } else {
            string value = std::to_string(rand() % total);
            bufferFile << operation << " " << key << " " << 0 << " " << value << "\n";
        }

    }

    bufferFile.close();

}

/**
 * all comments are following https://developer.lsst.io/cpp/api-docs.html standard
 */

vector<levelMeta> getMetadata() {
    vector<levelMeta> ret;
    return ret;
}
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
//FIXME: 
BufferClass buffer;
LevelClass level;
LSM lsm;
void driver(int operation, int key = 0, string value = "", int targetKey = 0, int lowerBound = 0, int upperBound = 0) {
    int bufferSize = buffer.currentSize;
    vector<levelMeta> levelMetadata = getMetadata();
    // int totalLevel = levelMetadata.size();
    int currentLevel = 1;
    levelMeta curLevel = levelMetadata[currentLevel];
    int levelSize = levelMetadata.back().levelNumber;
    //TODO: runtime for print to substruct from overall runtime?
    switch (operation) {
        //TODO: make these all alive with manifest file
        case 0: {
            buffer.insert(key, value, false);
            cout << "inserted key " + to_string(key) + " and value" + value << endl;
            if (bufferSize == BUFFER_SIZE) {
                buffer.flush();
                currentLevel += 1;
                buffer.currentSize = 0;
            }
            if (curLevel.levelNumber == SIZE_RATIO) {
                // level.flushLevel(currentLevel);
                currentLevel += 1;
            }
            break;
        }
        case 1: {
            buffer.insert(key, value, false);
            cout << "updated key " + to_string(key) + " with value" + value << endl;
            if (bufferSize == BUFFER_SIZE) {
                buffer.flush();
                currentLevel += 1;
                buffer.currentSize = 0;
            }
            if (levelSize == SIZE_RATIO) {
                // level.flushLevel(currentLevel);
                currentLevel += 1;
            }
            break;
        }
        case 2: {
            buffer.insert(key, NULL, true); //FIXME:
            cout << "deleted key " + to_string(key) << endl;
            if (bufferSize == BUFFER_SIZE) {
                buffer.flush();
                currentLevel += 1;
                buffer.currentSize = 0;
            }
            if (levelSize == SIZE_RATIO) {
                // level.flushLevel(currentLevel);
                currentLevel += 1;
            }
            break;
        }
        case 3: {
            string pointLookup = buffer.searchKeyInBuffer(targetKey);
            if (pointLookup != "") 
                cout << "found key " + to_string(targetKey) + "'s value as " + pointLookup  << endl;
            else if (lsm.pointLookupLevel(targetKey) != "") {
                pointLookup = lsm.pointLookupLevel(targetKey);
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

int main(int argc, char *argv[]) {

    // Explain inputs
    if (argc != 5) {
    fprintf(stderr,"usage: %s case Q T  pol\n", argv[0]);
    fprintf(stderr,"\tCase - workload kind can be: Gral, Insert, Update/Del, PointR, RangeR \n");
    fprintf(stderr,"\tQ - buffer size\n");
    fprintf(stderr,"\tT - size ratio\n");
    fprintf(stderr,"\tPol - tiering vs levelling: can be t, l\n");
    fprintf(stderr,"\te.g. for an insert heavy workload with Q=4 and T=2 on tiering\n\t\t %s Insert 4 2 t\n",argv[0]);
    exit(1);
	}

    string wokload_kind = argv[1];
    int Q = atoi(argv[2]);
    int T = atoi(argv[3]);
    string Policy = argv[4];

    // Number of Instructions in Workload
    int total = 100000;

    // Build a workload according to size and kind of workload wanted, store it in workload.txt
    buildWorkload(wokload_kind, total);

    // iterate through the workload and process it via driver 
    std::ifstream infile("workload.txt");
    int operation, key1, key2;
    string value;
    while (infile >> operation >> key1 >> key2 >> value){
        driver(operation, key1, value, key1, key1, key2);
    }



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
    // 0.2.2 allocate space and initialize levelMeta for both approaches
    // KeyValuePair kv[2];
    // kv[0] = (KeyValuePair) {1, "1", true}; 
    // we expect a max of 10 levels in lsm
    // vector<levelMeta> meta;
    // meta.reserve(10);
    // vector<int> r;
    // r.push_back(8);
    // levelMeta m = {1, r, r};
    // meta[0] = m;
    // // write struct to file 
    // fwrite (&meta, sizeof(meta), 10, manifest); 
    // cout << sizeof(meta) << endl;
    // if(fwrite != 0)
    //     printf("contents to file written successfully !\n"); 
    // else
    //     printf("error writing file !\n"); 
    // // close file 
    // fclose (manifest); 
//######################################################################//
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
    // BC.insert(1, "9000", true); //FIXME: bc doesn't handle delete to null
    // BC.insert(7, "7", false);
    // BC.insert(9, "9", false);
    // BC.insert(10, "19", false);
    // BC.insert(10, "10", false);
    // BC.insert(15, "15", false);
    // BC.printBC();
    // LevelClass lv;
    // lv.currentLevel = 1;
    // lv.generateFilenameList();
    // vector<KeyValuePair> tmp = lv.leveling();
    // for (int i= 0; i<tmp.size(); i++) {
    //     cout << tmp[i].key << "-" << tmp[i].value << endl;
    // }
    // lv.printLV();
    string filename = "lsm_data/level_1_file_1.txt";
    // LSM lsm;
    // lsm.allLevel[1] = la;
    // lsm.pointLookup(1);
    return 1;
}

    // char command[50] = "cd lsm_data && ls -l";
    // system(command);

    /////////////////////////FILE EXAMPLE////////////////////////
    // FILE *infile; 
    // levelMeta input;
    // infile = fopen ("lsm_data/lsm.meta", "r"); 
    // if (infile == NULL) { 
    //     fprintf(stderr, "\nError opening file\n"); 
    //     exit (1); 
    // }  
    // // read file contents till end of file 
    // while(fread (&input, sizeof(input), 1, infile)) {
    //     cout << "level: " << to_string(input.levelNumber) << endl;
    //     // std::ostringstream oss;
    //     // vector<int> vec = input.lowerBound;
    //     // if (!vec.empty())
    //     // {
    //     //     // Convert all but the last element to avoid a trailing ","
    //     //     std::copy(vec.begin(), vec.end()-1,
    //     //         std::ostream_iterator<int>(oss, ","));

    //     //     // Now add the last element with no delimiter
    //     //     oss << vec.back();
    //     // }
    //     // std::cout << oss.str() << std::endl;
    //     // string str(input.lowerBound, input.lowerBound);
    //     // cout << "lower bound: " << str << endl;
    // }
    // // close file 
    // fclose (infile); 
    
    // ////////////////////////////////////////////////////////////////////////////////////
    
