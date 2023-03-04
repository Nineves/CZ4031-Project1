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
    friend class Node;

public:
    LLNode(unsigned int blockSize);
    ~LLNode(){

    };

    void insert(Record *newAddress);
    int getMaxNumAddress(unsigned int nodeCapacity);
    int getCurNumAddress(){
        return curNumAddress;
    }
    vector<char*> getAllAddress(){
        vector<char*> addrList = {};

        LLNode* curNode = this;
        while (curNode != nullptr)
        {
            for (int i = 0; i < curNode->curNumAddress; i++)
            {
                addrList.push_back((char*)(curNode->recordAddresses[i]));
            }
            curNode = curNode->nextNode;
        }
        
        return addrList;
    }
};

class Node
{
public:
    int maxNumOfKeys;
    int curNumOfKeys;
    int curNumOfPointers;
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
    LLNode* deleteLeafKey(int key, Storage *storage);
    void doShift(int start);
    void doReverseShift(int start);
    void updateKey(int preKey, int curKey);
    void updateDeletedParents(int key);
    Node(unsigned int blockSize, bool isLeaf);
    ~Node();
    int getMaxKeyNum();
    void printNode();
    void moveLeafKey(int ind, int key, LLNode* recordAddr);
    float calculateAve(int key);
    LLNode* getLLNode(int key);
};

Node::~Node()
{
}

#endif