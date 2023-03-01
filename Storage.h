#include "Block.h"
#include "Record.h"
#ifndef STORAGE_H_INCLUDED
#define STORAGE_H_INCLUDED
#pragma pack(1)

using namespace std;

class Storage {
private:
    unsigned int storage_size; //total available size of the memory for storing bl
    unsigned int blk_size; // the size of each block (in Bytes)
    unsigned int total_nof_blk; // total number of blocks available in the
    unsigned int allocated_nof_blk; // number of blocks allocated
    unsigned int allocated_size; // size allocated to store items
    unsigned int cur_blk_id;
    Block* blks; // a list of block information, storing available entry in each block

    void* storage_ptr; // point to the start address of the memory

    vector<pair<char*, int>> deleted_slots; // record the start address and size of deleted slots

public:
    Storage(){};
    Storage(unsigned int storage_size, unsigned int blk_size);
    Storage* addr_of_object(void) { return this; }
    pair<int, char*> get_available_ptr(int store_size);
    bool allocate_block();
    void release_blk();
    unsigned int get_blk_id(char* addr);
    char* insert_item(void* item_addr, int item_size);
    void delete_item(char* addr, int size_to_delete);
    void update_item(char* addr, int size_to_update, void* new_item);
    Record retrieve_record(char* addr);
    char* record_addr_to_blk_addr(char* raddr);
    vector<Record> retrieve_blk(char* blk_addr);

    char** getAllBlocks();
    
    void clear_storage();

    int get_storage_size();
    int get_blk_size();
    int get_allocated_nof_blk();
    int get_allocated_size();
    void* get_storage_ptr();
};

#endif // STORAGE_H_INCLUDED
