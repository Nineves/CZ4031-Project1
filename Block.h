#ifndef BLOCK_H_INCLUDED
#define BLOCK_H_INCLUDED

#include <cmath>
#include <iostream>
#include <vector>

using std::cout;
class Block {
    public:
    unsigned int blk_size;
    unsigned int space_used;
    unsigned int cur_avail_ptr;

    Block(unsigned int blk_size);
    unsigned int get_space_used();
    unsigned int get_avail_ptr();
    void store(int store_size);
    void delete_size(int size_to_delete);
};

#endif // BLOCK_H_INCLUDED
