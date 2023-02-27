#include <cmath>
#include <iostream>
#include <vector>
#include "BlockInfo.h"

BlockInfo::BlockInfo(unsigned int blk_size) {
    this->blk_size = blk_size;
    this->space_used = 0;
    this->cur_avail_ptr = 0;
}

unsigned int BlockInfo::get_avail_ptr() {
    return cur_avail_ptr;
}

unsigned int BlockInfo::get_space_used() {
    return space_used;
}

void BlockInfo::store(int store_size) {
    space_used += store_size;
    cur_avail_ptr += store_size;
}

void BlockInfo::delete_size(int size_to_delete) {
    space_used -= size_to_delete;
}
