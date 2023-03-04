#include <iostream>
#include <vector>

#include "defined_structures.h"
#include "Record.h"

using namespace std;

#ifndef NODE_H
#define NODE_H

class LLNode
{
private:
    int maxNumAddress;
    int curNumAddress;
    unsigned int blockSize;

    vector<Record *> recordAddresses;
    LLNode *nextNode;
    LLNode *nodePointer;

    friend class BPTree;

public:
    LLNode(unsigned int blockSize);
    ~LLNode(){

    };

    void insert(Record *newAddress);
    int getMaxNumAddress(unsigned int nodeCapacity);
};

class Node
{
private:
    int maxNumOfKeys;
    int curNumOfKeys;
    unsigned int blockSize;
    Node *parentAddr;
    Node *nextLeafNode;
    Node *lastLeafNode;
    Node *leafHead;
    int *keys;

    union
    {
        Node **nodePointers;
        LLNode **dataPointers;
    } ptrs;

    bool isLeaf;

    friend class BPTree;

public:
    int getMaxKeys(unsigned int nodeCapacity);
    void insertNonLeafKey(int key, Node *newNodeAddress);
    int insertLeafKey(int key, Record *recordAddress);
    void deleteNonLeafKey(int key);
    void deleteLeafKey(int key);
    void doShift(int start);
    void doReverseShift(int start);
    void updateKey(int preKey, int curKey);
    void updateDeletedParents(int key);
    Node(unsigned int blockSize, bool isLeaf);
    ~Node();
    int getMaxKeyNum();
    void printNode();
};

Node::~Node()
{
}

#endif