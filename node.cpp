#include "node.h"
#include "defined_structures.h"

#include <iostream>

using namespace std;

Node::Node(size_t blockSize, bool isLeaf)
{
    size_t nodeCapacity = blockSize - sizeof(bool) - 2 * sizeof(int) - sizeof(Node*) - sizeof(size_t);
    Node::maxNumOfKeys = getMaxKeys(nodeCapacity);
    this->curNumOfKeys = 0;
    this->isLeaf = isLeaf;
    keys = (int*) malloc(maxNumOfKeys * sizeof(int));

    //Intialize pointers for leaf nodes
    if (isLeaf)
    {
        ptrs.dataPointers = (LLNode**)malloc((maxNumOfKeys + 1) * sizeof(LLNode*));
        for (int i = 0; i <maxNumOfKeys + 1; i++)
        {
            ptrs.dataPointers[i] = nullptr;
        }
    }

    //Initialize pointers for non-leaf nodes
    else
    {
        ptrs.nodePointers = (Node**)malloc((maxNumOfKeys + 1) * sizeof(Node*));
        for (int i = 0; i <maxNumOfKeys + 1; i++)
        {
            ptrs.nodePointers[i] = nullptr;
        }
    }

    for (int i = 0; i < maxNumOfKeys; i++)
    {
        keys[i] = -1;
    }

}

int Node::getMaxKeys(size_t nodeCapacity)
{
    /*
        Calculate the maximum number of keys that a node can hold.
    */
    int maxkey = 0;

    if (isLeaf)
    {
        size_t sum = sizeof(LLNode*);
        if (sum + sizeof(LLNode*) + sizeof(int) > nodeCapacity) {
        throw std::overflow_error("Error: Keys and pointers cannot be put into a node!");
        }

        while (sum + sizeof(LLNode*) + sizeof(int) <= nodeCapacity) {
            maxkey ++;
            sum += sizeof(LLNode*) + sizeof(int);
        }  
    }

    else
    {
        size_t sum = sizeof(Node*);
        if (sum + sizeof(Node*) + sizeof(int) > nodeCapacity) {
        throw std::overflow_error("Error: Keys and pointers cannot be put into a node!");
        }

        while (sum + sizeof(Node*) + sizeof(int) <= nodeCapacity) {
            maxkey ++;
            sum += sizeof(Node*) + sizeof(int);
        }
    }
    return maxkey;
}

int Node::getMaxKeyNum()
{
    return maxNumOfKeys;
}

void Node::insertNonLeafKey(int key, Node *newNodeAddress) {
    /*
        Insert new key and pointer into a non-leaf node.
    */
    for (int i = 0; i < maxNumOfKeys; i++)
        {
            if (keys[i] > key and i < maxNumOfKeys - 1)
            {
                doShift(i);
                keys[i] = key;
                ptrs.nodePointers[i + 1] = newNodeAddress;
                curNumOfKeys ++;
                break;
            }

            if (i == maxNumOfKeys - 1)
            {
                keys[i] = key;
                ptrs.nodePointers[i+1] = newNodeAddress;
                curNumOfKeys ++;
                break;
            }
        }
}

int Node::insertLeafKey(int key, Record* recordAddress) {
    int flag = 0;
    for(int i = 0; i < maxNumOfKeys; i++){
        if (keys[i] > key and i < maxNumOfKeys - 1 )
        {
            if (i == 0)
            {
                flag = 1;
            }
            
            doShift(i);
            keys[i] = key;
            LLNode * newLLNode = new LLNode(blockSize);
            newLLNode->insert(recordAddress);
            ptrs.dataPointers[i] = newLLNode;
            curNumOfKeys ++;
            break;
        }

        else if (keys[i] == key)
        {
            LLNode *headNode = ptrs.dataPointers[i];
            headNode->insert(recordAddress);
            break;
        }
    }
    return flag;
}

void Node::doShift(int start) {
    /*
    Shift both keys and pointers
    */
   if (isLeaf)
   {
        for(int j = curNumOfKeys - 1; j > start; j--){
            keys[j] = keys[j-1];
            ptrs.dataPointers[j + 1] = ptrs.dataPointers[j];
        }
   
   }
   else
   {
        for(int j = curNumOfKeys - 1; j > start; j--){
        keys[j] = keys[j-1];
        ptrs.nodePointers[j] = ptrs.nodePointers[j-1];
        }
   }
}

void Node::updateKey(int preKey, int curKey) {
    for (int i = 0; i < curNumOfKeys; i++)
    {
        if (keys[i] == preKey)
        {
            keys[i] = curKey;
            cout << "Key updated in parent node." << endl;
            break;
        }
    }
    cout << "Key is not found." << endl;
}

LLNode::LLNode(size_t blockSize) 
{
    size_t nodeCapacity = blockSize - sizeof(bool) - 2 * sizeof(int) - sizeof(size_t);
    recordAddresses = (Record**) malloc(maxNumAddress * sizeof(Record*));
    this->nextNode = nullptr;
    this->blockSize = blockSize;
    this->curNumAddress = 0;

    this->maxNumAddress = getMaxNumAddress(nodeCapacity);
}

void LLNode::insert(Record *newAddress)
{
    LLNode *curNode = this;
    while (nextNode != nullptr)
    {
        curNode = nextNode;
    }

    //Create new LLNode
    if (curNumAddress == maxNumAddress)
    {
        LLNode *newLLNode = new LLNode(blockSize);
        newLLNode->insert(newAddress);
        nextNode = newLLNode;
    }
    
    else
    {
        recordAddresses[curNumAddress] = newAddress;
        curNumAddress += 1;
    }
}

int LLNode::getMaxNumAddress(size_t nodeCapacity) {
    int maxNum = 0;
    size_t sum = sizeof(LLNode*);
    if (sum > nodeCapacity) {
        throw std::overflow_error("Error: Address cannot be put into a node!");
    }

    while (sum + sizeof(Record*) <= nodeCapacity) {
        maxNum ++;
        sum += sizeof(Record*);
    }  
}