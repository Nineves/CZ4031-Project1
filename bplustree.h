#include <iostream>
#include <vector>
#include "node.h"
#include "defined_structures.h"
#include "Storage.h"
#include "Record.h"

using namespace std;

#ifndef BPTREE_H
#define BPTREE_H

class BPTree 
{
private:

    Storage *memory;
    Node *root;                                     //Root node in memory (if loaded)
    
    int maxNumOfKeys, numOfNodes, numOfLevels;
    size_t blockSize;


public:

    BPTree(size_t nodeSize);

    void insert(int key, Record *recordAddress);

    void remove(int key);

    int split(Node *nodeToSplit, int key, Node* insertNode);

    Node* searchLeafNode(int key);

    void searchSingle(int key);

    void searchRange(int lowerBound, int higherBound);

    Node* getRoot();

    void displayNode(Node* curNode);

    void updateParents(int newKey, Node *parentAddr, Node *childAddr);

    void createNewRoot(Node *node1, Node *node2)
 
};

#endif