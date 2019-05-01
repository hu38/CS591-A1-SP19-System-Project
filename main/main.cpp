#include "BufferClass.cpp"
#include "LevelClass.cpp"
#include "LSM.cpp"
using namespace std;
#include <typeinfo>
#include <cstdlib>
#define TIMER chrono::high_resolution_clock

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
    int probabilities[20] = {0,0,0,0,1,1,1,1,1,2,2,2,2,3,3,3,3};

    if (workload_kind == "Gral"){
        probabilities[0] = 0;
    } else if (workload_kind == "Insert"){
        probabilities[5] = 0;
        probabilities[7] = 0;
        probabilities[9] = 0;
        probabilities[13] = 0;
        probabilities[15] = 0;
    } else if (workload_kind == "Del"){
        probabilities[0] = 1;
        probabilities[9] = 1;
        probabilities[13] = 1;
        probabilities[15] = 1;
    } else if (workload_kind == "PointR"){
        probabilities[0] = 2;
        probabilities[4] = 2;
        probabilities[6] = 2;
        probabilities[13] = 2;
        probabilities[15] = 2;
    } else {
        probabilities[0] = 3;
        probabilities[1] = 3;
        probabilities[7] = 3;
        probabilities[9] = 3;
        probabilities[11] = 3;
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
    int total = 1000;

    // Build a workload according to size and kind of workload wanted, store it in workload.txt
    buildWorkload(workload_kind, total);

    // iterate through the workload and process it via driver 
    ifstream infile("workload.txt");
    int operation, key1, key2;
    string value;
    
    chrono::duration<double> elapsed;
    if (Policy == "t"){
        while (infile >> operation >> key1 >> key2 >> value){
            TIMER::time_point start = TIMER::now();
            lsm.driverTiering(operation, key1, value, key1, key1, key2, Q, T);
            TIMER::time_point ending = TIMER::now();
            elapsed = elapsed + (ending - start);
        }
        for (int i = 0; i< lsm.LSMTier.size(); i++) {
            for (int j=0; j<lsm.LSMTier[i].tierData.size(); j++) 
                cout << "level " << i << " ranges from " <<  lsm.LSMTier[i].tierData[j].minkey << " to " << lsm.LSMTier[i].tierData[j].maxkey << endl;
        }
    } else {
        // lsm.currentLevel = 0;
        while (infile >> operation >> key1 >> key2 >> value){
            TIMER::time_point start = TIMER::now();
            lsm.driverLeveling(operation, key1, value, key1, key1, key2, Q, T); 
            TIMER::time_point ending = TIMER::now();
            elapsed = elapsed + (ending - start);
        }
        for (int i = 0; i< lsm.LSMLevel.size(); i++) {
            cout << "level " << i << " ranges from " <<  lsm.LSMLevel[i].keyRange[0] << " to " << lsm.LSMLevel[i].keyRange[1] << endl;
        }
    }

    cout << elapsed.count()/total << endl;
    
    
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
