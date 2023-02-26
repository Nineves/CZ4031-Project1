#ifndef STRUCTURE_H
#define STRUCTURE_H

struct Address
{
    void *blockAddr;
    int offset;
};

struct DataRecord
{
    int numVotes;
    float avgRatinig;
    char tconst[10];
};


#endif