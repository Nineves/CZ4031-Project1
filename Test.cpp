#include <iostream>
#include <cmath>
#include <iostream>
#include "Storage.h"
#include "bplustree.h"
#include "bplustree.cpp"
#include "Storage.cpp"
#include "Calculation.cpp"
#include "node.h"
#include "node.cpp"
#include <cstdlib>
#include <fstream> //ifstream
#include <sstream>  // stringstream
#include <iostream> // cout, endl
#include <iomanip>  // ws
#include <map>      // map
#include <string>

using namespace std;

void store_records(Storage* storage);
void RunExperiment1(Storage *storage);
BPTree* RunExperiment2(Storage *storage);
void buildBPTree(Storage *storage, BPTree *bPlusTree);
void reportBPTreeStatistics(BPTree* bPlusTree);

Calculation cals;
int blockSize;
int storage_size;
Storage *storage;
BPTree *bPlusTree;

int main() {
    // Record* r1 = new Record(1, 2 ,1);
    // Record* r2 = new Record(1,2,2);
    // Record* r3 = new Record(1,2,3);
    // Record* r4 = new Record(1,2,4);
    // Record* r5 = new Record(1,2,5);
    // Record* r6 = new Record(1,2,6);
    // Record* r7 = new Record(1,2,7);
    // Record* r8 = new Record(1,2,8);
    // Record* r9 = new Record(1,2,9);
    // Record* r10 = new Record(1,2,10);


    // BPTree *bpt = new BPTree(50); 
    // bpt->insert(1,r1);
    // bpt->insert(2,r2);
    // bpt->insert(3,r3);
    // bpt->insert(4,r4);
    // bpt->insert(5,r5);
    // bpt->insert(6,r6);
    // bpt->insert(7,r7);
    // bpt->insert(8,r8);
    // bpt->insert(9,r9);
    // bpt->insert(10,r10);

    
    // bpt->printNode(bpt->getRoot());

     blockSize = 200;

    storage_size = cals.GetMaxSizeOfRecordBlocks(blockSize);
    int reservation = 1000000;
    storage = Storage(storage_size + reservation, blockSize).addr_of_object();
    cout << "Initialization complete. Block size has been set to " << blockSize << "B.\n";
    cout << "Allocation complete. Disk storage of size " << ((storage_size + reservation) / (double)1000000) << "MB is now allocated.\n";
    cout << "Allocated storage is now empty, please run experiment 1 to load data.\n"<<endl;

    RunExperiment1(storage);

    RunExperiment2(storage);


    return 0;

}

void RunExperiment1(Storage *storage)
{
    cout << "Loading data...\n";
    store_records(storage);
    cout << "Loading data complete.\n";
}

void store_records(Storage *storage)
{
    int skip_header = 0;
    int record_number = 0;
    ifstream infile("data.tsv");
    string line;
    while (getline(infile, line))
    {
        if (skip_header == 0)
        {
            skip_header = 1;
            continue;
        }
        string string_component;
        stringstream record_string(line);
        int name;
        int16_t average_rating_int;
        int numofvotes;
        for (int i = 0; i < 3; i++)
        {
            getline(record_string, string_component, '\t');
            if (i == 0)
            {
                string_component.erase(0, 2);
                name = stoi(string_component);
            }
            else if (i == 1)
            {
                float average_rating_float = stof(string_component);
                average_rating_int = static_cast<int16_t>(average_rating_float * 10);
            }
            else
            {
                numofvotes = stoi(string_component);
            }
        }
        Record temp(name, (int16_t)average_rating_int, numofvotes);

        storage->insert_item(&temp, RECORD_SIZE);
        record_number++;
    }
    infile.close();

    cout << "Total storage space: " << (double)storage->get_storage_size() / 1000000.0 << " MB" << endl;
    cout << "Block size: " << storage->get_blk_size() << " Bytes" << endl;
    cout << "Record size: " << RECORD_SIZE << " Bytes" << endl;
    cout << "Allocated number of blocks: " << storage->get_allocated_nof_blk()<< endl;
    cout << "Allocated size: " << (double)storage->get_allocated_size() / 1000000.0 << " MB"  << endl;

    return;
}

BPTree* RunExperiment2(Storage *storage)
{
    cout << "Building B+ Tree...\n";
    BPTree* bPlusTree = new BPTree(storage->get_blk_size());
    buildBPTree(storage, bPlusTree);
    reportBPTreeStatistics(bPlusTree);
    return bPlusTree;
}

void buildBPTree(Storage *storage, BPTree* bPlusTree)
{   
    int count = 0;
    char** allBlks = storage->getAllBlocks();
    int n = storage->get_allocated_nof_blk();
    vector<Record> curRecords;

    for (int i = 0; i < n; i++)
    {
        char* curBlock = allBlks[i];
        curRecords = storage->retrieve_blk(curBlock);
        int numOfRecords = curRecords.size();
        int key;
        for (int j = 0; j < numOfRecords; j++)
        {
            key = curRecords[j].getNumOfVotes();           
            Record record = curRecords[j];
            bPlusTree->insert(key, &record);
            count++;
            //printf("COUNT  %d\n",count);
        }
    }

}

void reportBPTreeStatistics(BPTree* bPlusTree)
{
    cout<<"Number of Levels: "<<bPlusTree->getNumOfLevels()<<endl;
    cout<<"Number of Nodes: "<<bPlusTree->getNumOfNodes()<<endl;
    cout<<"Maximum Number of Keys in a Node: "<<bPlusTree->getN()<<endl;
    bPlusTree->printNode(bPlusTree->getRoot());
}