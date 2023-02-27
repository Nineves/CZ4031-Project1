#ifndef BLOCKINFO_H_INCLUDED
#define BLOCKINFO_H_INCLUDED

#include <cmath>
#include <iostream>
#include <vector>

using std::cout;
class BlockInfo {
    public:
    unsigned int blk_size;
    unsigned int space_used;
    unsigned int cur_avail_ptr;

    BlockInfo(unsigned int blk_size);
    unsigned int get_space_used();
    unsigned int get_avail_ptr();
    void store(int store_size);
    void delete_size(int size_to_delete);
};

#endif // BLOCKINFO_H_INCLUDED
