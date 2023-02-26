#include "node.h"
#include "defined_structures.h"

#include <iostream>

using namespace std;

Node::Node(size_t blockSize)
{
    size_t nodeCapacity = blockSize - sizeof(bool) - 2 * sizeof(int);
    Node::maxNumOfKeys = getMaxKeys(nodeCapacity);
}

int Node::getMaxKeys(size_t nodeCapacity)
{
    int maxkey = 0;
    size_t sum = sizeof(Address);
    if (sum + sizeof(Address) + sizeof(int) > nodeCapacity) {
        throw std::overflow_error("Error: Keys and pointers cannot be put into a node!");
    }

    while (sum + sizeof(Address) + sizeof(int) <= nodeCapacity) {
        maxkey ++;
        sum += sizeof(Address) + sizeof(int);
    }  
}

int Node::getMaxKeyNum()
{
    return maxNumOfKeys;
}

LLNode::LLNode(Address *nodeAddr, size_t blockSize) 
{
    size_t nodeCapacity = blockSize - sizeof(bool) - 2 * sizeof(int);
    this->nodeDiskAddresses.push_back(nodeAddr);
    this->nextNode = nullptr;
    this->blockSize = blockSize;
    this->curNumAddress = 1;

    this->maxNumAddress = getMaxNumAddress(nodeCapacity);
}

void LLNode::insert(Address *newAddress)
{
    LLNode *curNode = this->nextNode;
    while (curNode->nextNode != nullptr)
    {
        curNode = curNode->nextNode;
    }

    //Create new LLNode
    if (curNode->curNumAddress == curNode->maxNumAddress)
    {
        LLNode *newLLNode = new LLNode(newAddress, blockSize);
        curNode->nextNode = newLLNode;
    }
    
    else
    {
        curNode->nodeDiskAddresses.push_back(newAddress);
    }
}

int getMaxNumAddress(size_t nodeCapacity) {
    int maxNum = 0;
    size_t sum = sizeof(LLNode*);
    if (sum > nodeCapacity) {
        throw std::overflow_error("Error: Address cannot be put into a node!");
    }

    while (sum + sizeof(Address) <= nodeCapacity) {
        maxNum ++;
        sum += sizeof(Address);
    }  
}