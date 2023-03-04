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
#include <set>
#include <fstream> //ifstream
#include <sstream>  // stringstream
#include <iostream> // cout, endl
#include <iomanip>  // ws
#include <map>      // map
#include <string>
#include <chrono> 
#pragma pack(1)

using std::cout;
using namespace std::chrono;

void store_records(Storage* storage);
void RunExperiment1(Storage *storage);
BPTree* RunExperiment2(Storage *storage);
void buildBPTree(Storage *storage, BPTree *bPlusTree);
void reportBPTreeStatistics(BPTree* bPlusTree);
int bruteForce(int target, Storage *storage);
int bruteForce_range(int start, int end, Storage *storage);
float calcAverage(vector<Record> records);
int getNumOfBlksAccessed(vector<char*> recordAddrs, Storage* storage);


void RunExperiment3(Storage* storage, BPTree *bPlusTree);
void RunExperiment4(Storage* storage, BPTree *bPlusTree);
void RunExperiment5(Storage *storage, BPTree *bPlusTree, int key);

void bruteForceSearch(Storage *storage, int start, int end = 0);
void report_bPlusTree_statistics(BPTree *bPlusTree, int block_size, bool parameter_n, bool num_nodes, bool height, bool content);
void delete_records(Storage *storage, BPTree *bPlusTree, int key);
void delete_key_in_index(BPTree *bPlusTree, int key);
void delete_records_in_db(Storage *storage, vector<char *> record_addresses);

vector<char *> get_all_record_addr(Node *start_node, int numOfNodeAccess, int start, int end = 0);


void retrieve_search_statistics_storage(Storage *storage, vector<char *> search_results_addresses);


// void retrieve_search_statistics_index(bplustree *bPlusTree, Node *start_node, int start, int end = 0);
// vector<Node *> get_nodes_accessed_at_leaf_level(Node *start_node, int start, int end);




//global variables
Calculation cals;
int blockSize;
int storage_size;
Storage *storage;
BPTree *bPlusTree;

int main()
{

    blockSize = 200;

    storage_size = cals.GetMaxSizeOfRecordBlocks(blockSize);
    int reservation = 1000000;
    storage = Storage(storage_size + reservation, blockSize).addr_of_object();
    cout << "Initialization complete. Block size has been set to " << blockSize << "B.\n";
    cout << "Allocation complete. Disk storage of size " << ((storage_size + reservation) / (double)1000000) << "MB is now allocated.\n";
    cout << "Allocated storage is now empty, please run experiment 1 to load data.\n"<<endl;
    char sel = 'n';

    RunExperiment1(storage);
    BPTree* bpt = RunExperiment2(storage);
    RunExperiment3(storage, bpt);
    RunExperiment4(storage, bpt);


    while (sel != '6')
    {
        cout << "Please select one of the options below. (1, 2, 3, 4, 5, or 6)\n";
        cout << "1. Run experiment 1\n"
             << "2. Run experiment 2\n"
             << "3. Run experiment 3\n"
             << "4. Run experiment 4\n"
             << "5. Run experiment 5\n"
             << "6. Terminate program\n";

        bool validSelection = true;
        do{
            validSelection = true;
            cout << "Selection: ";
            cin >> sel;
            cout << "You have selected option " << sel << ".\n"<<endl;
            switch (sel)
            {
            case '1':
            {
                    cout << "Running experiment 1...\n";
                    RunExperiment1(storage);
                    cout << "Completed experiment 1...\n"<<endl;
                    break;
            }
            case '2':
            {
                cout << "Running experiment 2...\n";
                bPlusTree = RunExperiment2(storage);
                cout << "Completed experiment 2...\n"<<endl;
                break;
            }
            case '3':
            {
                cout << "Running experiment 3...\n";
                RunExperiment3(storage, bPlusTree);
                cout << "Completed experiment 3...\n"<<endl;
                break;
            }
            case '4':
            {
                cout << "Running experiment 4...\n";
                RunExperiment4(storage, bPlusTree);
                cout << "Completed experiment 4...\n"<<endl;
                break;
            }
            case '5':
             {
                cout << "Running experiment 5...\n";
                // code to run experiment 5
                RunExperiment5(storage, bPlusTree, 1000);
                cout << "Completed experiment 5...\n"<<endl;
                break;
             }
            case '6':
            {
                cout << "Terminating program....\n";
                // clean up code if any
                cout << "Bye!"<<endl;
                break;
            }
            default:
            {
                validSelection = false;
                cout << "Your selection is invalid, please select a valid option. (0, 1, 2, 3, 4, 5, or, 6)\n"<<endl;
            }
            }
        } while (!validSelection);
    }
}

void RunExperiment1(Storage *storage)
{
    cout << "Loading data...\n";
    store_records(storage);
    cout << "Loading data complete.\n";
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
    int offset = storage->get_blk_size();
    char *curr_block_ptr = (char *)storage->get_storage_ptr();
    int num_allocated_blocks = storage->get_allocated_nof_blk();
    for (int i = 0; i < num_allocated_blocks; i++)
    {
        vector<Record> curr_block_records = storage->retrieve_blk(curr_block_ptr);
        for (int j = 0; j < curr_block_records.size(); j++)
        {
            Record curr_record = curr_block_records.at(j);
            int numVotes = curr_record.getNumOfVotes();

            char *original_record_pointer = curr_block_ptr + j * (sizeof(Record));
            bPlusTree->insert(numVotes, (Record*)original_record_pointer);
        }
        curr_block_ptr += offset;
    }
}

void reportBPTreeStatistics(BPTree* bPlusTree)
{
    cout<<"Number of Levels: "<<bPlusTree->getNumOfLevels()<<endl;
    cout<<"Number of Nodes: "<<bPlusTree->getNumOfNodes()<<endl;
    cout<<"Maximum Number of Keys in a Node: "<<bPlusTree->getN()<<endl;
    bPlusTree->printNode(bPlusTree->getRoot());
}

void RunExperiment3(Storage* storage, BPTree *bPlusTree)
{
    int target = 500;
    auto time1 = high_resolution_clock::now();

    Node *leafNode = bPlusTree->searchLeafNode(target);
    LLNode *recordList = leafNode->getLLNode(target);
    vector<char*> recordAddrs = recordList->getAllAddress();
    vector<Record> records = {};

    int sum = 0;
    for(int i=0; i < recordAddrs.size(); i ++){
        Record curRecord = storage->retrieve_record(recordAddrs[i]);
        records.push_back(curRecord);
    }
    auto time2 = high_resolution_clock::now();

    float aveNum = calcAverage(records);

    int numOfNodeAccess = bPlusTree->getNumOfNodeSearch(target);
    int numOfBlocksAccess_tree = getNumOfBlksAccessed(recordAddrs, storage);

    auto time3 = high_resolution_clock::now();
    int numOfBlocksAccess_brutal = bruteForce(target, storage);
    auto time4 = high_resolution_clock::now();

    auto duration1 = duration_cast<microseconds>(time2 - time1);
    auto duration2 = duration_cast<microseconds>(time4 - time3);

    cout << "The number of index nodes accessed: "<< numOfNodeAccess << endl;
    cout << "The number of data blocks accessed: "<< numOfBlocksAccess_tree <<endl;
    cout << "The average rating is: "<< aveNum <<endl;
    cout << "The running time of the search & retrieval process (with B+ Tree): " << duration1.count() << " microseconds" << endl;
    cout << "The running time of the search & retrieval process (in brute force): " << duration2.count() << " microseconds" << endl;
    cout << "The number of data blocks accessed (in brutal force): " << storage->get_allocated_nof_blk() << endl;

}

void RunExperiment4(Storage* storage, BPTree *bPlusTree)
{
    int start_of_range = 30000;
    int end_of_range = 40000;
    set<int> blk_index;
    int blk;

    vector<LLNode*> LLNodeList = {};
    auto time1 = high_resolution_clock::now();
    Node *start_node = bPlusTree->searchRangeLeafNode(start_of_range);
    Node *cur_node = start_node;
    vector<Record> records = {};
    while (cur_node->keys[0] <= end_of_range)
    {
        for (int i = 0; i < cur_node->curNumOfKeys; i++)
        {
            if (cur_node->keys[i] >= start_of_range && cur_node->keys[i] <= end_of_range)
            {
                LLNode *curLLNode = cur_node->ptrs.dataPointers[i];
                vector<char*> recordAddrs = curLLNode->getAllAddress();
                
                for(int i=0; i < recordAddrs.size(); i ++){                 
                    blk = storage->get_blk_id(recordAddrs[i]);
                    blk_index.insert(blk);
                    Record curRecord = storage->retrieve_record(recordAddrs[i]);
                    records.push_back(curRecord);
                }
            }
        }
        cur_node = cur_node->nextLeafNode;
        
        
        if (cur_node == nullptr)
        {
            cout<<"Reached end"<<endl;
            break;
        }
        
        
    }
    auto time2 = high_resolution_clock::now();
    
    cout<<records.size()<<endl;
    float aveNum = calcAverage(records);

    int numOfNodeAccess = bPlusTree->getNumOfNodeSearch(start_of_range);
    int numOfBlocksAccess_tree = blk_index.size();

    auto time3 = high_resolution_clock::now();
    int numOfBlocksAccess_brutal = bruteForce_range(start_of_range, end_of_range, storage);
    auto time4 = high_resolution_clock::now();

    auto duration1 = duration_cast<microseconds>(time2 - time1);
    auto duration2 = duration_cast<microseconds>(time4 - time3);

    cout << "The number of index nodes accessed: "<< numOfNodeAccess << endl;
    cout << "The number of data blocks accessed: "<< numOfBlocksAccess_tree <<endl;
    cout << "The average rating is: "<< aveNum <<endl;
    cout << "The running time of the search & retrieval process (with B+ Tree): " << duration1.count() << " microseconds" << endl;
    cout << "The running time of the search & retrieval process (in brute force): " << duration2.count() << " microseconds" << endl;
    cout << "The number of data blocks accessed (in brutal force): " << storage->get_allocated_nof_blk() << endl;
}

int getNumOfBlksAccessed(vector<char*> recordAddrs, Storage* storage)
{
    set<int> blk_index;
    int blk;
    for (int i = 0; i < recordAddrs.size(); i++)
    {
        blk = storage->get_blk_id(recordAddrs[i]);
        blk_index.insert(blk);
    }
    return blk_index.size();
    
};

float calcAverage(vector<Record> records) {
    float sum = 0;
    
    for (int i = 0; i < records.size(); i++)
    {
        sum +=  records[i].rating2Dec();
        //cout<< records[i].rating2Dec()<<endl;
        
    }
    

    return sum/records.size();
    
}

int bruteForce(int target, Storage *storage) {
    int count = 0;
    int flag = 0;
    int sum = 0;
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
            if (key == target)
            {
                sum += record.getAvgRating();
            }
            
            //printf("COUNT  %d\n",count);
        }
        count ++;
    }

    return count;
}

int bruteForce_range(int start, int end, Storage *storage) {
    int count = 0;
    int flag = 0;
    int sum = 0;
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
            if (key <end && key > start)
            {
                sum += record.getAvgRating();
            }
            
            //printf("COUNT  %d\n",count);
        }
        count ++;
    }

    return count;
}


void RunExperiment5(Storage *storage, BPTree *bPlusTree, int key)
{
    bPlusTree->remove(key);
    reportBPTreeStatistics(bPlusTree);

    // delete_records(storage, bPlusTree, key);
    // report_bPlusTree_statistics(bPlusTree, storage->get_blk_size(), false, true, true, true);
    // cout << "Number of times that a node is deleted = " << num_nodes_deleted << "\n"
    // << "Note: it does not include the number of Parrays deleted (level between leaf nodes and records)"<<endl;
}


// experiment 1 helper code
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

// experiment 5
void delete_records_in_db(Storage *storage, vector<char *> record_addresses)
{

    for (int i = 0; i < record_addresses.size(); i++)
    {
        storage->delete_item(record_addresses.at(i), sizeof(record_addresses.at(i)));
    }
}

// void delete_key_in_index(BPTree *bPlusTree, int key)
// {
//     bool res = bPlusTree->Delete(key);
//     if (res)
//     {
//         cout << "Deleted key " << key << " in B+ tree\n";
//     }
//     else
//     {
//         cout << "Failed to delete key " << key << " in B+ tree\n";
//     }
// }

// void delete_records(Storage *storage, bplustree *bPlusTree, int key)
// {
//     Node *start_node = bPlusTree->SearchLeafNode(key);
//     vector<char *> record_addresses = get_all_record_addr(start_node, key);
//     cout << "Found all records with numVotes = " << key << "\n";
//     delete_records_in_db(storage, record_addresses); // this one can return a tuple detailing the number of times a node is deleted
//     cout << "Deleted all records with numVotes = " << key << "\n";
//     delete_key_in_index(bPlusTree, key);
//     cout << "Finished deleting records from storage and updating B+ tree\n";
// }
