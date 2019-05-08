#include "BufferClass.cpp"
#include "LevelClass.cpp"
#include "LSM.cpp"
using namespace std;

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
    int probabilities[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};


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
        if (operation == 3) {
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

    if (workload_kind == "Gral"){
        probabilities[0] = 0;
        probabilities[1] = 1;
        probabilities[2] = 2;
        probabilities[3] = 3;
    } else if (workload_kind == "Insert"){
        probabilities[0] = 0;
        probabilities[1] = 0;
        probabilities[2] = 1;
        probabilities[3] = 2;
    } else if (workload_kind == "Update/Del"){
        probabilities[0] = 0;
        probabilities[9] = 1;
        probabilities[13] = 1;
        probabilities[15] = 2;
    } else if (workload_kind == "PointR"){
        probabilities[0] = 0;
        probabilities[1] = 2;
        probabilities[2] = 2;
        probabilities[3] = 2;
    } else {
        probabilities[0] = 0;
        probabilities[1] = 3;
        probabilities[2] = 2;
        probabilities[3] = 3;
    }

    for (i = 0; i < total; i++){
        int randIndex = rand() % 4;
        int operation = probabilities[randIndex];
        int key = rand() % (total/2);
        string value = std::to_string(rand() % 1000);
        if (operation == 3) {
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
    int total = 500000/2;

    // Build a workload according to size and kind of workload wanted, store it in workload.txt
    buildWorkload(workload_kind, total);

    // iterate through the workload and process it via driver 
    ifstream infile("workload.txt");
    int operation, key1, key2;
    string value;
    // iterate through the workload and process it via driver 
    chrono::duration<double> elapsed;
    TIMER::time_point start = TIMER::now();
    if (Policy == "t"){
        while (infile >> operation >> key1 >> key2 >> value){
            lsm.driverTiering(operation, key1, value, key1, key1, key2, Q, T);
        }
         // for (int i = 0; i< lsm.LSMTier.size(); i++) {
        //     for (int j=0; j<lsm.LSMTier[i].tierData.size(); j++) 
        //         cout<< "level " << i  << "tier " << j << " ranges from " <<  lsm.LSMTier[i].tierData[j].minkey << " to " << lsm.LSMTier[i].tierData[j].maxkey << endl;
        // }
    } else {
        lsm.currentLevel = 0;
        while (infile >> operation >> key1 >> key2 >> value){
            lsm.driverLeveling(operation, key1, value, key1, key1, key2, Q, T); 
        }
        // for (int i = 0; i< lsm.LSMLevel.size(); i++) {
        //     cout << "level " << i << " ranges from " <<  lsm.LSMLevel[i].keyRange[0] << " to " << lsm.LSMLevel[i].keyRange[1] << " at "<< lsm.LSMLevel[i].filename << " has " << lsm.LSMLevel[i].totalNumberOfPairs << " pais." << endl;
        // }
    }
    TIMER::time_point ending = TIMER::now();
    elapsed = elapsed + (ending - start);
    cout << elapsed.count() / 2 << endl;

    return 1;
}
