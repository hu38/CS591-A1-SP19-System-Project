#include "BufferClass.cpp"
#include "LevelClass.cpp"
#include "LSM.cpp"
using namespace std;
#include <typeinfo>
#include <cstdlib>

/**
 * all comments are following https://developer.lsst.io/cpp/api-docs.html standard
 */

/**
 * generates a workload based on user inputs (what type of workload - general/evenly distributed, 
 * insert heavy, update/delete heavy, point lookup heavy, or range lookup heavy.)
 * 
 * @param[workload_kind:required] instruction for the above workload type in form of string (Gral,
 * Insert, Update/Del, PointR, RangeR respectively)
 * @param[total:required] the number of total records desired to generate
 */
void buildWorkload(string workload_kind, int total){
    int probabilities[20] = {0,0,0,0,1,1,2,2,3,3,3,3,4,4,4,4};

    if (workload_kind == "Gral"){
        probabilities[0] = 0;
    } else if (workload_kind == "Insert"){
        probabilities[5] = 0;
        probabilities[7] = 0;
        probabilities[9] = 0;
        probabilities[13] = 0;
        probabilities[15] = 0;
    } else if (workload_kind == "Update/Del"){
        probabilities[0] = 1;
        probabilities[9] = 2;
        probabilities[13] = 1;
        probabilities[15] = 2;
    } else if (workload_kind == "PointR"){
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
    std::ofstream workloadfile ("workload.txt");
    for (i = 0; i < total; i++){
        int randIndex = rand() % 20;
        int operation = probabilities[randIndex];
        int key = rand() % (total/2);
        string value = std::to_string(rand() % 1000);
        if (operation == 4) {
            int key2 = rand() % total/2;
            if (key > key2) {
                workloadfile << operation << " " << key2 << " " << key << " " << value << "\n";
            } else {
                workloadfile << operation << " " << key << " " << key2 << " " << value << "\n";
            }
        } else {
            string value = std::to_string(rand() % total);
            workloadfile << operation << " " << key << " " << 0 << " " << value << "\n";
        }
    }
    workloadfile.close();
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
// void driverLeveling(int operation, int key = 0, string value = "", int targetKey = 0, int lowerBound = 0, int upperBound = 0, int Q =0, int T = 0) {
//     // int bufferSize = buffer.currentSize;
//     bool flushed = false;
//     vector<levelMetadata> currentLSM = lsm.LSMLevel;
//     // vector<levelMetadata> levelMetadata = getMetadata();
//     //TODO: runtime for print to substruct from overall runtime? + add timer
//     switch (operation) {
//         //TODO: make these all alive with manifest file
//         case 0: {
//             cout << "i'm inserting key " << key << " and value " << value << endl;
//             buffer.insert(key, value, false);
//             if (buffer.currentSize == BUFFER_SIZE) {
//                 string newFilename = buffer.flushLevel(currentLSM.size() + 1);
//                 int ranges[] = {buffer.smallest, buffer.largest};
//                 levelMetadata newPage = {lsm.currentLevel, newFilename, *ranges, 1};
//                 newPage.totalNumberOfPairs = buffer.totalNonDup;
//                 currentLSM.push_back(newPage);
//                 lsm.currentLevel++;
//                 buffer.totalNonDup = 0;
//                 int levelSize = currentLSM[lsm.currentLevel].totalNumberOfPairs;
//                 //     cout << currentLSM[i].filename << endl;
//                 //     if (kv.size() >= (Q * pow(T, lsm.currentLevel))) {
//                 //         cout << "we should have a new level" << endl;
//                 //         lsm.currentLevel --;
//                 //     }
//                 //     if (currentLSM[i].totalNumberOfPairs >= (Q * (T^i))){
//                 //         lsm.flushLevel(i);
//                 //         lsm.currentLevel ++;
//                 //         cout << "new level: " << lsm.currentLevel << endl;
//                 //     } else {
//                 //         cout << "here!" << endl;
//                 //         break;
//                 //     }
//             }
            
//             break;
//         }
//         case 1: {
//             buffer.insert(key, value, false);
//             cout << "updated key " + to_string(key) + " and value" + value << endl;
//             if (buffer.currentSize == BUFFER_SIZE) {
//                 // buffer.flush();
//                 buffer.currentSize = 0;
//                 flushed = true;

//             }
//             if (flushed == true){
//                 int numOfLevels = lsm.LSMLevel.size();
//                 for (int i = 0; i < numOfLevels; i++){
//                     if (lsm.LSMLevel[i].totalNumberOfPairs >= (Q * (T^i))){
//                         lsm.flushLevel(i);
//                     } else {
//                         break;
//                     }
//                 }
//             }
            
//             break;
//         }
//         case 2: {
//             buffer.insert(key, "", true);
//             cout << "deleted key " + to_string(key) << endl;
//             if (buffer.currentSize == BUFFER_SIZE) {
//                 // buffer.flush();
//                 buffer.currentSize = 0;
//                 flushed = true;

//             }
//             if (flushed == true){
//                 int numOfLevels = lsm.LSMLevel.size();
//                 for (int i = 0; i < numOfLevels; i++){
//                     if (lsm.LSMLevel[i].totalNumberOfPairs >= (Q * (T^i))){
//                         lsm.flushLevel(i);
//                     } else {
//                         break;
//                     }
//                 }
//             }
            
//             break;
//         }
//         case 3: {
//             string pointLookup = buffer.searchKeyInBuffer(targetKey);
//             if (pointLookup != "") 
//                 cout << "found key " + to_string(targetKey) + "'s value as " + pointLookup  << endl;
//             else if (lsm.pointLookupLevel(targetKey) != "") {
//                 pointLookup = lsm.pointLookupLevel(targetKey);
//                 cout << "found key " + to_string(targetKey) + "'s value as " + pointLookup  << endl;
//             } else {
//                 cout << "key not found"  << endl;
//             }
//             break;
//         }
//         case 4: {
//             vector<string> rangeLookup = lsm.rangeLookupLevel(lowerBound, upperBound);
//             if (rangeLookup.size() > 0) {
//                 cout << "found: ";
//                 for (int i=0; i< rangeLookup.size(); i++) {
//                     cout << rangeLookup[i];
//                 }
//                 cout << endl;
//             }
//             else {
//                 cout << "key not found"  << endl;
//             }
//             break;
//         }
//         default: {
//             cout << "requested operation code " << to_string(operation) << " not found" << endl; 
//         }
//     }
// }

// LSM driverTiering(int operation, int key = 0, string value = "", int targetKey = 0, int lowerBound = 0, int upperBound = 0, int Q = 0, int T= 0, LSM lsm) {
//     int bufferSize = buffer.currentSize;
//     bool flushed = false;

//     // vector<levelMeta> levelMetadata = getMetadata();
//     // // int totalLevel = levelMetadata.size();
//     // int currentLevel = 1;
//     // levelMeta curLevel = levelMetadata[currentLevel];
//     // int levelSize = levelMetadata.back().levelNumber;
//     //TODO: runtime for print to substruct from overall runtime?
//     switch (operation) {
//         //TODO: make these all alive with manifest file
//                 // string newFilename = buffer.flushLevel(currentLSM.size() + 1);
//                 // int ranges[] = {buffer.smallest, buffer.largest};
//                 // levelMetadata newPage = {lsm.currentLevel, newFilename, *ranges, 1};
//                 // newPage.totalNumberOfPairs = buffer.totalNonDup;
//                 // currentLSM.push_back(newPage);
//                 // lsm.currentLevel++;
//                 // buffer.totalNonDup = 0;
//                 // int levelSize = currentLSM[lsm.currentLevel].totalNumberOfPairs;
//                 //     cout << currentLSM[i].filename << endl;
//                 //     if (kv.size() >= (Q * pow(T, lsm.currentLevel))) {
//                 //         cout << "we should have a new level" << endl;
//                 //         lsm.currentLevel --;
//                 //     }
//                 //     if (currentLSM[i].totalNumberOfPairs >= (Q * (T^i))){
//                 //         lsm.flushLevel(i);
//                 //         lsm.currentLevel ++;
//                 //         cout << "new level: " << lsm.currentLevel << endl;
//                 //     } else {
//                 //         cout << "here!" << endl;
//                 //         break;
//                 //     }
//             }
            
//             break;
//         case 0: {
//             buffer.insert(key, value, false);
//             cout << "inserted key " + to_string(key) + " and value" + value << endl;
//             if (bufferSize == BUFFER_SIZE) {
//                 // buffer.flush();
//                 buffer.currentSize = 0;
//                 flushed = true;

//             }
//             if (flushed == true){
//                 int numOfTiers = lsm.LSMTier.size();
//                 for (int i = 0; i < numOfTiers; i++){
//                     if (lsm.LSMTier[i].totalNumberOfTiers >= T){
//                         lsm.flushLevel(i);
//                     } else {
//                         break;
//                     }
//                 }
//             }
            
//             break;
//         }
//         case 1: {
//             buffer.insert(key, value, false);
//             cout << "updated key " + to_string(key) + " and value" + value << endl;
//             if (bufferSize == BUFFER_SIZE) {
//                 // buffer.flush();
 
//                 buffer.currentSize = 0;
//                 flushed = true;

//             }
//             if (flushed == true){
//                 int numOfTiers = lsm.LSMTier.size();
//                 for (int i = 0; i < numOfTiers; i++){
//                     if (lsm.LSMTier[i].totalNumberOfTiers >= T){
//                         lsm.flushLevel(i);
//                     } else {
//                         break;
//                     }
//                 }
//             }
            
//             break;
//         }
//         case 2: {
//             buffer.insert(key, "", true);
//             cout << "deleted key " + to_string(key) << endl;
//             if (bufferSize == BUFFER_SIZE) {
//                 // buffer.flush();
//                 buffer.currentSize = 0;
//                 flushed = true;

//             }
//             if (flushed == true){
//                 int numOfTiers = lsm.LSMTier.size();
//                 for (int i = 0; i < numOfTiers; i++){
//                     if (lsm.LSMTier[i].totalNumberOfTiers >= T){
//                         lsm.flushLevel(i);
//                     } else {
//                         break;
//                     }
//                 }
//             }
            
//             break;
//         }
//         case 3: {
//             string pointLookup = buffer.searchKeyInBuffer(targetKey);
//             if (pointLookup != "") 
//                 cout << "found key " + to_string(targetKey) + "'s value as " + pointLookup  << endl;
//             else if (lsm.pointLookupLevel(targetKey) != "") {
//                 pointLookup = lsm.pointLookupLevel(targetKey);
//                 cout << "found key " + to_string(targetKey) + "'s value as " + pointLookup  << endl;
//             } else {
//                 cout << "key not found"  << endl;
//             }
//             break;
//         }
//         case 4: {
//             vector<string> rangeLookup = lsm.rangeLookupTier(lowerBound, upperBound);
//             if (rangeLookup.size() > 0) {
//                 cout << "found: ";
//                 for (int i=0; i< rangeLookup.size(); i++) {
//                     cout << rangeLookup[i];
//                 }
//                 cout << endl;
//             }
//             else {
//                 cout << "key not found"  << endl;
//             }
//             break;
//         }
//         default: {
//             cout << "requested operation code " << to_string(operation) << " not found" << endl; 
//         }
//     }
// }


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

    string workload_kind = argv[1];
    int Q = atoi(argv[2]);
    int T = atoi(argv[3]);
    string Policy = argv[4];

    BufferClass buffer;
    LevelClass level;
    LSM lsm;

    // Number of Instructions in Workload
    int total = 12;

    // Build a workload according to size and kind of workload wanted, store it in workload.txt
    //buildWorkload(workload_kind, total);

    // iterate through the workload and process it via driver 
    std::ifstream infile("workload.txt");
    int operation, key1, key2;
    string value;

    if (Policy == "t"){
        while (infile >> operation >> key1 >> key2 >> value){
            lsm.driverTiering(operation, key1, value, key1, key1, key2, Q, T);
        }
    } else {
        lsm.currentLevel = 0;
        while (infile >> operation >> key1 >> key2 >> value){
            lsm.driverLeveling(operation, key1, value, key1, key1, key2, Q, T); 
        }
    }

    // vector<KeyValuePair> tmp = buffer.readFile("lsm_data/level_1_file_1.txt");
    // cout << tmp.size() << endl;
    // 0. preparation
    // 0.1 create a folder for LSM data with read and write privileges
    // if folder exists, recreating won't swipe off files inside and start from scratch
    mkdir("lsm_data/", S_IRWXU); 
    // 0.2 create a manifest file to log LSM execuations
    // FILE *manifest;
    // manifest = fopen ("main/lsm_data/lsm.meta", "w"); 
    // if (manifest == NULL) {
    //     fprintf(stderr, "\nError opend file\n"); 
    //     exit (1); 
    // }
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
	
    // BC.insert(400, "9", false);
    // BC.insert(700, "50", false);
    // cout << BC.currentSize << endl;
    // BC.insert(1, "9000", true);
    // BC.insert(7, "7", false);
    // BC.insert(9, "9", false);
    // BC.insert(10, "19", false);
    // BC.insert(10, "10", false);
    // BC.insert(15, "15", false);
    // buffer.insert(300, "3", false);
    // buffer.insert(100, "1", false);
    // buffer.insert(100, "6", false);
    // buffer.insert(200, "2", false);
    // buffer.insert(40, "4", false);
    // buffer.flushLevel(2);
    // BC.printBC();
    // LevelClass lv;
    // lv.currentLevel = 1;
    // lv.currentSize = 0;
    // lv.generateFilenameList();
    // lv.tiering();
    // lv.printLV();
    // for (int i= 0; i<tmp.size(); i++) {
    //     cout << tmp[i].key << "-" << tmp[i].value << endl;
    // }
    // string filename = "lsm_data/level_1_file_4.txt";
    // vector<KeyValuePair> tmp = lv.readFile(filename);
    // bool tmp = lv.currentSize == 20;
    // cout << tmp << ", current size: " << lv.currentLevel << endl;
    // LSM lsm;
    // lsm.allLevel[1] = la;
    // lsm.pointLookup(1);
    // cout << searchKey(tmp, 19) << endl;
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
