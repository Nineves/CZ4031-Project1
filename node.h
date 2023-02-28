#include <iostream>
#include <vector>

#include "defined_structures.h"
#include "Record.h" 

using namespace std;

#ifndef NODE_H
#define NODE_H

class Node
{
private:
    int maxNumOfKeys;
    int curNumOfKeys;
    size_t blockSize;
    Node *parentAddr;
    Node *nextLeafNode;
    int* keys;

    union {
        Node ** nodePointers;
        LLNode ** dataPointers;
    }ptrs;


    bool isLeaf;

    friend class BPTree; 

public:
    int getMaxKeys(size_t nodeCapacity);
    void insertNonLeafKey(int key, Node *newNodeAddress);
    int insertLeafKey(int key, Record* recordAddress);
    void doShift(int start);
    void updateKey(int preKey, int curKey);
    Node(size_t blockSize, bool isLeaf);
    ~Node();
    int getMaxKeyNum();
};

Node::~Node()
{
}


class LLNode
{
private:
    int maxNumAddress;
    int curNumAddress;
    size_t blockSize;
    
    Record** recordAddresses;
    LLNode* nextNode;
    LLNode* nodePointer;

    friend class BPTree;

public:
    LLNode(size_t blockSize);
    ~LLNode();

    void insert(Record *newAddress);
    int getMaxNumAddress(size_t nodeCapacity);
};








#endif