#include <iostream>
#include <vector>

#include "defined_structures.h"

using namespace std;

#ifndef NODE_H
#define NODE_H

class Node
{
private:
    int maxNumOfKeys;
    int curNumOfKeys;
    Address *parentAddr;
    vector<int> keys;
    vector<Address*> pointers;

    bool isLeaf;

    friend class BPTree; 

    int getMaxKeys(size_t nodeCapacity);

public:
    Node(size_t blockSize);
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
    
    vector<Address*> nodeDiskAddresses;
    LLNode* nextNode;
    LLNode* nodePointer;

    friend class BPTree;

public:
    LLNode(Address *nodeAddr, size_t blockSize);

    void insert(Address *newAddress);
};








#endif