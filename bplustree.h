#include <iostream>
#include <vector>
#include "node.h"
#include "defined_structures.h"

using namespace std;

#ifndef BPTREE_H
#define BPTREE_H

class BPTree 
{
private:

    Memory *dataMem, indexMem;
    Node *root;                                     //Root node in memory (if loaded)
    Address *rootAddress;
    int maxNumOfKeys, numOfNodes, numOfLevels;
    size_t blockSize;

    void insertInNode(Node *node, int key, Address *newNodeAddress);
    void insertInnerNode(Node *curNode, int key, Address *childAddr);
    void doShift(Node *node, int start);
    void doInnerShift(Node *node, int start);
    void updateParents();

public:

    BPTree(size_t nodeSize, Memory *dataMem, Memory *indexMem);

    void insert(int key, Address *recordAddress);

    void remove(int key);

    void searchSingle(int key);

    void searchRange(int lowerBound, int higherBound);

    Node* getRoot();

    void displayNode(Node* curNode);

    void updateParents(int newKey, Address *parentAddr, Address *childAddr);
 
};

#endif