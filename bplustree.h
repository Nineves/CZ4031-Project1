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
    Node *leafHead;
    Node *root; // Root node in memory (if loaded)

    int maxNumOfKeys, numOfNodes, numOfLevels;
    unsigned int blockSize;

public:
    BPTree(unsigned int nodeSize);

    int getN()
    {
        return maxNumOfKeys;
    }

    int getNumOfLevels()
    {
        return numOfLevels;
    }

    int getNumOfNodes()
    {
        return numOfNodes;
    }

    void insert(int key, Record *recordAddress);

    void remove(int key);

    int split(Node *nodeToSplit, int key, Node *insertNode);

    Node *searchInsertLeafNode(int key);

    Node *searchDeleteLeafNode(int key);

    Node *searchLeafNode(int key);

    int getNumOfNodeSearch(int key);

    Node *getRoot()
    {
        return root;
    };

    void displayNode(Node *curNode);

    void updateParents(int newKey, Node *parentAddr, Node *childAddr);

    void createNewRoot(Node *node1, Node *node2);

    void printNode(Node *treenode);

    void mergeLeaf(Node *sourceNode, Node *mergeNode);

    void mergeNonLeaf(Node *sourceNode, Node* mergeNode);

    bool CheckTree();

    void deleteKey(int key);

    bool CheckNode(Node *pNode);
};

#endif