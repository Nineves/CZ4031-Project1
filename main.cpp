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
#pragma pack(1)

using std::cout;
using namespace std::chrono;

void store_records(Storage* storage);
void RunExperiment1(Storage *storage);
BPTree* RunExperiment2(Storage *storage);
void buildBPTree(Storage *storage, BPTree *bPlusTree);
void reportBPTreeStatistics(BPTree* bPlusTree);



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
                //RunExperiment3(storage, bPlusTree);
                cout << "Completed experiment 3...\n"<<endl;
                break;
            }
            case '4':
            {
                cout << "Running experiment 4...\n";
                //RunExperiment4(storage, bPlusTree);
                cout << "Completed experiment 4...\n"<<endl;
                break;
            }
            case '5':
             {
                cout << "Running experiment 5...\n";
                // code to run experiment 5
                //RunExperiment5(storage, bPlusTree, 1000);
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

/*
bplustree* RunExperiment2(Storage *storage)
{
    bplustree* bPlusTree = new bplustree(cals.GetMaxNumOfKeysPerIndexBlock(storage->get_blk_size()));
    build_BPlus_tree(storage, bPlusTree);
    report_bPlusTree_statistics(bPlusTree, storage->get_blk_size(), true, true, true, true);
    return bPlusTree;
}
*/

// void RunExperiment3(Storage* storage, BPTree *bPlusTree)
// {
//     int key_to_find = 500;
//     auto time1 = high_resolution_clock::now();
//     Node *start_node = bPlusTree->searchLeafNode(key_to_find);
//     auto time2 = high_resolution_clock::now();
//     int numOfNodeAccess = bPlusTree->getNumOfNodeSearch(key_to_find);
//     auto time3 = high_resolution_clock::now();
//     vector<char *> record_addresses = get_all_record_addr(start_node, numOfNodeAccess, key_to_find);
//     auto time4 = high_resolution_clock::now();

//     retrieve_search_statistics_storage(storage, record_addresses);

//     auto duration1 = duration_case<microseconds>(time2 - time1);
//     auto duration2 = duration_case<microseconds>(time4 - time3);
//     cout << "The running time of the retrieval process: " << duration1.count() + duration2.count() << " microseconds" << endl;

//     bruteForceSearch(storage, key_to_find);
//     // retrieve_search_statistics_index(bPlusTree, start_node, key_to_find);
// }

// void RunExperiment4(Storage* storage, BPTree* bPlusTree)
// {
//     int start_of_range = 30000;
//     int end_of_range = 40000;
//     auto time1 = high_resolution_clock::now();
//     Node *start_node = bPlusTree->searchLeafNode(start_of_range);
//     auto time2 = high_resolution_clock::now();
//     int numOfNodeAccess = bPlusTree->getNumOfNodeSearch(start_of_range);
//     auto time3 = high_resolution_clock::now();
//     vector<char *> record_addresses = get_all_record_addr(start_node, numOfNodeAccess, start_of_range, end_of_range);
//     auto time4 = high_resolution_clock::now();

//     retrieve_search_statistics_storage(storage, record_addresses);

//     auto duration1 = duration_case<microseconds>(time2 - time1);
//     auto duration2 = duration_case<microseconds>(time4 - time3);
//     cout << "The running time of the retrieval process: " << duration1.count() + duration2.count() << " microseconds" << endl;

//     bruteForceSearch(storage, start_of_range, end_of_range);
//     // code for number and content of index nodes the process accesses
//     // retrieve_search_statistics_index(bPlusTree, start_node, start_of_range, end_of_range);
// }

void RunExperiment5(Storage *storage, BPTree *bPlusTree, int key)
{
    bPlusTree->remove(key);

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


// void report_bPlusTree_statistics(BPTree *bPlusTree, int block_size, bool parameter_n, bool num_nodes, bool height, bool content)
// {
//     Calculation cals = Calculations();
//     if (parameter_n)
//     {
//         cout << "Parameter n of B+ tree: " << cals.GetMaxNumOfKeysPerIndexBlock(block_size) << endl; // could change depending on BPlusTree/CNode attributes
//     }
//     if (num_nodes)
//     {
//         CNode* rootnode = bPlusTree->GetRoot();
//         cout << "Number of nodes of the B+ tree: " << bPlusTree->NumofNode(rootnode) + 1 << "\n";
//     }
//     if (height)
//     {
//         int h = 0;
//         CNode* curr_node = bPlusTree->GetRoot();
//         while (curr_node->GetType() != NODE_TYPE_LEAF)
//         {
//             h++; // number of internal nodes in path
//             curr_node = curr_node->GetPointer(1);
//         }
//         h++;
//         cout << "Height of B+ tree = " << h << "\n";
//     }
//     if (content)
//     {
//         // code to print content of root node and its first child node
//         CNode *root = bPlusTree->GetRoot();
//         CNode *child = nullptr;
//         if (root->GetType()!=NODE_TYPE_LEAF){
//             child = root->GetPointer(1);
//         }
//         cout << "Root node keys: ";
//         for (int i = 1; i <= root->GetCount();i++){
//             cout << root->GetElement(i) << " | ";
//         }
//         cout << "\n";
//         if (child == nullptr){
//             cout << "Root node is a leaf node and therefore has no child;\n";
//         }
//         else{
//             cout << "First child node keys: ";
//             for (int i = 1; i <= child->GetCount(); i++){
//                 cout << child->GetElement(i) << " | ";
//             }
//             cout << "\n";
//         }
//     }
// }

// experiment 3 and 4
// vector<char *> get_all_record_addr(Node *start_node, int numOfNodeAccess, int start, int end)
// {

//     if (start_node == nullptr){
//         cout << "This shouldnt happen";
//     }
//     int numOfNodeAccessed = numOfNodeAccess;

//     vector<char*> record_addr = {};
//     vector<Parray*> parrays = {};

//     if (end == 0){
//         end = start;
//     }

//     int curr_key = start;
//     Node *curr_node = start_node;
//     bool flag_for_while = true;
//     do{
//         int i = 1;
//         for (i = 1; (i <= curr_node->GetCount()); i++)
//         {
//             int curr_key_in_node = curr_node->keys[i];
//             if (curr_key_in_node >= start && curr_key_in_node <= end){
//                 parrays.push_back(curr_node->ptrs.dataPointers[i]); //TODO: nodePointer or dataPointer?
//             }
//             if (curr_key_in_node > end)
//             {
//                 flag_for_while = false;
//                 break;
//             }
//         }
//         // cout << "size of parrays " << parrays.size() << "\n";
//         curr_node = curr_node->nextLeafNode;
//         if (curr_node == nullptr){
//             break;
//         } else {
//             numOfNodeAccessed++;
//         }
//     } while (flag_for_while);

//     //copy to recordaddr
//     for (int i = 0; i < parrays.size(); i++){

//         Parray* curr_parray = parrays.at(i);

//         vector<Record *> array = curr_parray->getarray();
//         for (int j = 0; j < array.size(); j++){
//             record_addr.push_back((char*)array.at(j));
//         }
//     }
//     // cout << "number of record address:" << record_addr.size() << "\n";
//     cout << "The number of index nodes the process accesses: " << numOfNodeAccessed << endl;
//     return record_addr;
// }

// void retrieve_search_statistics_storage(Storage *storage, vector<char *> search_results_addresses)
// {
//     int block_count = 0;
//     int record_count = 0;
//     float average_rating = 0;
//     char* prev_block_address = nullptr;
//     int index = 0;
//     while(index<search_results_addresses.size())
//     {
//         char* current_block_address = storage->record_addr_to_blk_addr(search_results_addresses[index]);
//         if (current_block_address == prev_block_address)
//         {
//             //retrieve the avg rating of the individual record
//             Record temp_record = storage->retrieve_record(search_results_addresses[index]);
//             average_rating += temp_record.rating2Dec();
//             record_count += 1;
//         }
//         else{
//             if(block_count<5)
//             {
//                 cout << "This is block number " << block_count + 1 <<" and the block ID is " <<  storage->get_blk_id(current_block_address) <<endl;
//                 cout << endl;
//                 cout <<"Tconst of records in the block are"<<endl;
//                 cout <<endl;
//                 vector<Record> contents_of_block = storage->retrieve_blk(current_block_address);
//                 for (int i = 0; i< contents_of_block.size() ; i++)
//                  {
//                      cout <<contents_of_block[i].getTconst()<< " | ";
//                  }
//                 cout <<endl;
//             }
//             //retrieve the avg rating of the individual record
//             Record temp_record = storage->retrieve_record(search_results_addresses[index]);
//             average_rating += temp_record.rating2Dec();
//             // increment record count no matter what, but block count only if its not consecutively the same
//             record_count += 1;
//             block_count +=1;
//         }
//         //change the prev to current for next loop
//         prev_block_address = current_block_address;
//         index++;
//     }
//     cout << "The number of data blocks the process accesses: " << block_count<< endl;
//     //the contents of first 5 printed out beforehand in the if portion
//     cout << "The number of records found: " << record_count <<endl;
//     //cout<< "This is the total sum" << average_rating <<endl;
//     cout << "The average of \"averageRating\'s\" of the records that are returned: " << average_rating/record_count <<endl;
//     return;
// }

// void bruteForceSearch (Storage *storage, int start, int end) {
//     cout << "========== Brute-froce method ==========" << endl;

//     if (end == 0 ) {
//         end = start;
//     }

//     auto time1 = high_resolution_clock::now();

//     int count = 0;
//     char** allBlks = storage->getAllBlocks();
//     int n = storage->get_allocated_nof_blk();
//     vector<Record> contents_of_block;

//     for (int i = 0; i < n; i++)
//     {
//         char* curBlock = allBlks[i];
//         contents_of_block = storage->retrieve_blk(curBlock);
//         int numOfRecords = curRecords.size();
//         int key;
//         for (int j = 0; j < numOfRecords; j++)
//         {
//             key = contents_of_block[j].getNumOfVotes();
//             if ((key >= start) && (key <= end)){
//                 count++;
//                 cout << "Tconst: " << contents_of_block[j].getTconst() << "; numVotes: " << key << endl;
//             }
//         }
//     }
//     auto time2 = high_resolution_clock::now();
//     auto duration1 = duration_case<microseconds>(time2 - time1);

//     cout << "The number of data blocks accessed by brute-force method: " << n << endl; // TODO: just all the blocks right?
//     cout << "The running time of brute-force method: " << duration1 << endl;
//     return;
// }

/*
void retrieve_search_statistics_index(bplustree* bPlusTree, Node* start_node, int start, int end){
    if (end == 0)
    {
        end = start;
    }
    vector<CNode *> nodes_accessed_to_find_start = bPlusTree->AncestoryOfLeafNode(start);
    vector<Node*> leaf_nodes_accessed_after_finding_start = get_nodes_accessed_at_leaf_level(start_node, start, end);

    int total_index_blocks_accessed = nodes_accessed_to_find_start.size() + leaf_nodes_accessed_after_finding_start.size();
    cout << "total_index_blocks_accessed = " << total_index_blocks_accessed << "\n";
    int printcount = 0;
    int max_printcount = 5;
    CNode *curr_node = nullptr;
    for (int j = 0; j < nodes_accessed_to_find_start.size();j++){
        curr_node = nodes_accessed_to_find_start.at(j);
        if (curr_node == nullptr){
            cout<<"SOMETHING IS WRONG\n";
            return;
        }
        cout << "Keys index block " << printcount+1 << " accessed: ";
        for (int i = 1; i <= curr_node->GetCount();i++){
            cout << curr_node->GetElement(i)<<" | ";
        }
        cout << "\n";
        printcount++;
        if (printcount==5){
            break;
        }
    }
    for (int j = 0; j < leaf_nodes_accessed_after_finding_start.size(); j++)
    {
        if (printcount == 5){
            break;
        }
        curr_node = leaf_nodes_accessed_after_finding_start.at(j);
        if (curr_node == nullptr)
        {
            cout << "SOMETHING IS WRONG\n";
            return;
        }
        cout << "Keys index block " << printcount + 1 << " accessed: ";
        for (int i = 1; i <= curr_node->GetCount(); i++)
        {
            cout << curr_node->GetElement(i) << " | ";
        }
        cout << "\n";
        printcount++;
    }
}
vector<Node*> get_nodes_accessed_at_leaf_level(Node* start_node, int start, int end){
    vector<Node *> nodes_accessed = {}; // excepted start_node
    if (start == end) {
        return nodes_accessed;
    }
    int curr_key = start;
    Node *curr_node = start_node;
    bool flag_for_while = true;
    do
    {
        int i = 1;
        for (i = 1; (i <= curr_node->GetCount()); i++)
        {
            int curr_key_in_node = curr_node->GetElement(i);
            if (curr_key_in_node >= start && curr_key_in_node <= end)
            {
                // parrays.push_back(curr_node->GetPointer1(i));
            }
            if (curr_key_in_node > end)
            {
                flag_for_while = false;
                break;
            }
        }
        curr_node = curr_node->m_pNextNode;
        if (curr_node == nullptr)
        {
            break;
        }
        nodes_accessed.push_back(curr_node);

    } while (flag_for_while);
    return nodes_accessed;
}
*/

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
