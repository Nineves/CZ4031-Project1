#include "node.h"
#include "defined_structures.h"
#include "constants.h"

#include <iostream>

using namespace std;

Node::Node(unsigned int blockSize, bool isLeaf)
{
    unsigned int nodeCapacity = blockSize - sizeof(bool) - 3 * sizeof(int) - INDEX_POINTER_SIZE;
    Node::maxNumOfKeys = getMaxKeys(nodeCapacity);
    this->curNumOfKeys = 0;
    this->curNumOfPointers = 0;
    this->isLeaf = isLeaf;
    this->blockSize = blockSize;
    this->nextLeafNode = nullptr;
    this->lastLeafNode = nullptr;

    keys = (int *)malloc(maxNumOfKeys * sizeof(int));

    // Intialize pointers for leaf nodes
    if (isLeaf)
    {
        ptrs.dataPointers = (LLNode **)malloc((maxNumOfKeys + 1) * sizeof(LLNode *));
        for (int i = 0; i < maxNumOfKeys + 1; i++)
        {
            ptrs.dataPointers[i] = nullptr;
        }
    }

    // Initialize pointers for non-leaf nodes
    else
    {
        ptrs.nodePointers = (Node **)malloc((maxNumOfKeys + 1) * sizeof(Node *));
        for (int i = 0; i < maxNumOfKeys + 1; i++)
        {
            ptrs.nodePointers[i] = nullptr;
        }
    }

    for (int i = 0; i < maxNumOfKeys; i++)
    {
        keys[i] = -1;
    }
}

int Node::getMaxKeys(unsigned int nodeCapacity)
{
    /*
        Calculate the maximum number of keys that a node can hold.
    */
    int maxkey = 0;

    unsigned int sum = INDEX_POINTER_SIZE;
    if (sum + INDEX_POINTER_SIZE + INDEX_KEY_SIZE > nodeCapacity)
    {
        throw std::overflow_error("Error: Keys and pointers cannot be put into a node!");
    }

    while (sum + INDEX_POINTER_SIZE + INDEX_KEY_SIZE <= nodeCapacity)
    {
        maxkey++;
        sum += INDEX_POINTER_SIZE + INDEX_KEY_SIZE;
    }
    return maxkey;
}

int Node::getMaxKeyNum()
{
    return maxNumOfKeys;
}

void Node::printNode()
{
    for (int i = 0; i < maxNumOfKeys; i++)
    {
        printf("\n");
        printf("%d ", keys[i]);
        cout << ptrs.nodePointers[i];
        printf("\n");
    }
    cout << ptrs.nodePointers[maxNumOfKeys] << endl;
}

void Node::insertNonLeafKey(int key, Node *newNodeAddress)
{
    /*
        Insert new key and pointer into a non-leaf node.
    */

    newNodeAddress->parentAddr = this;
    if (curNumOfKeys == 0)
    {
        keys[0] = key;
        ptrs.nodePointers[1] = newNodeAddress;

        curNumOfKeys++;
        curNumOfPointers++;

        return;
    }

    for (int i = 0; i < curNumOfKeys; i++)
    {
        if (keys[i] > key)
        {
            doShift(i);
            keys[i] = key;
            ptrs.nodePointers[i + 1] = newNodeAddress;

            curNumOfKeys++;
            curNumOfPointers++;
            break;
        }

        if (i == curNumOfKeys - 1)
        {
            keys[curNumOfKeys] = key;
            ptrs.nodePointers[curNumOfKeys + 1] = newNodeAddress;

            curNumOfKeys++;
            curNumOfPointers++;
            break;
        }
    }
}

int Node::insertLeafKey(int key, Record *recordAddress)
{
    int flag = 0;
    if (curNumOfKeys == 0)
    {
        keys[0] = key;
        LLNode *newLLNode = new LLNode(blockSize);
        newLLNode->insert(recordAddress);
        ptrs.dataPointers[0] = newLLNode;
        curNumOfKeys++;
        curNumOfPointers++;

        return 1;
    }

    for (int i = 0; i < curNumOfKeys; i++)
    {
        if (keys[i] > key)
        {
            doShift(i);
            keys[i] = key;
            LLNode *newLLNode = new LLNode(blockSize);
            newLLNode->insert(recordAddress);
            ptrs.dataPointers[i] = newLLNode;
            curNumOfKeys++;
            curNumOfPointers++;

            break;
        }

        else if (keys[i] == key)
        {
            LLNode *headNode = ptrs.dataPointers[i];
            headNode->insert(recordAddress);

            break;
        }
        else if (i == curNumOfKeys - 1)
        {
            keys[curNumOfKeys] = key;
            LLNode *newLLNode = new LLNode(blockSize);
            newLLNode->insert(recordAddress);
            ptrs.dataPointers[curNumOfKeys] = newLLNode;
            curNumOfKeys++;
            curNumOfPointers++;

            break;
        }
    }

    return flag;
}

int Node::deleteLeafKey(int key)
{
    for (int i = 0; i < curNumOfKeys; i++)
    {
        if (keys[i] == key)
        {
            doReverseShift(i);
            // keys.pop_back();
            delete ptrs.dataPointers[curNumOfKeys];
            //ptrs.dataPointers[curNumOfKeys] = nullptr;
            curNumOfKeys--;
            curNumOfPointers--;
            return i;
        }
    }
}

void Node::moveLeafKey(int ind, int key, LLNode* recordAddr)
{
    if (ind == this->curNumOfKeys)
    {
        keys[ind] = key;
        ptrs.dataPointers[ind] = recordAddr;
        curNumOfKeys++;
        curNumOfPointers++;
        return;
    }
    else
    {
        doShift(ind);
        keys[ind] = key;
        ptrs.dataPointers[ind] = recordAddr;
        curNumOfKeys++;
        curNumOfPointers++;
    }
    
    
}


void Node::doShift(int start)
{
    /*
    Shift both keys and pointers
    */
    if (start == maxNumOfKeys - 1)
    {
        return;
    }


    if (isLeaf)
    {
        for (int j = curNumOfKeys; j > start; j--)
        {
            keys[j] = keys[j - 1];
            ptrs.dataPointers[j] = ptrs.dataPointers[j - 1];
        }
    }
    else
    {
        for (int j = curNumOfKeys; j > start; j--)
        {
            keys[j] = keys[j - 1];
        }
        for (int i = curNumOfKeys + 1; i > start + 1; i--)
        {
            ptrs.nodePointers[i] = ptrs.nodePointers[i - 1];
        }
    }
}

void Node::doReverseShift(int start)
{
    if (isLeaf)
    {
        for (int j = start; j < curNumOfKeys; j++)
        {
            keys[j] = keys[j + 1];
            ptrs.dataPointers[j] = ptrs.dataPointers[j + 1];
        }
    }
    else
    {
        for (int j = start; j < curNumOfKeys; j++)
        {
            keys[j] = keys[j + 1];
        }
        for (int i = start + 1; i <= curNumOfKeys; i++)
        {
            ptrs.nodePointers[i] = ptrs.nodePointers[i + 1];
        }
    }
}

void Node::updateDeletedParents(int key)
{

    while (parentAddr != nullptr)
    {
        for (int i = 0; i < curNumOfKeys; i++)
        {
            if (parentAddr->keys[i] == key)
            {
                parentAddr->keys[i] = keys[0];
                break;
            }
        }
        parentAddr = parentAddr->parentAddr;
    }
}

LLNode* Node::getLLNode(int target){
    for (int i = 0; i < curNumOfKeys; i++)
    {
        if (target == keys[i])
        {
            return ptrs.dataPointers[i];
        }
    }
    return nullptr;
}


LLNode::LLNode(unsigned int blockSize)
{
    unsigned int nodeCapacity = blockSize - sizeof(bool) - 2 * sizeof(int) - sizeof(size_t);
    this->recordAddresses = {};
    this->nextNode = nullptr;
    this->blockSize = blockSize;
    this->curNumAddress = 0;

    this->maxNumAddress = getMaxNumAddress(nodeCapacity);
}

void LLNode::insert(Record *newAddress)
{
    LLNode *curNode = this;
    while (curNode->nextNode != nullptr)
    {
        curNode = curNode->nextNode;
    }

    // Create new LLNode
    if (curNode->curNumAddress == curNode->maxNumAddress)
    {
        LLNode *newLLNode = new LLNode(blockSize);
        newLLNode->recordAddresses.push_back(newAddress);
        newLLNode->curNumAddress += 1;
        curNode->nextNode = newLLNode;
    }

    else
    {
        curNode->recordAddresses.push_back(newAddress);
        curNode->curNumAddress += 1;
    }
    // cout<<"Insert successfully\n";
}

int LLNode::getMaxNumAddress(unsigned int nodeCapacity)
{
    int maxNum = 0;
    unsigned int sum = sizeof(LLNode *);
    if (sum > nodeCapacity)
    {
        throw std::overflow_error("Error: Address cannot be put into a node!");
    }

    while (sum + sizeof(Record *) <= nodeCapacity)
    {
        maxNum++;
        sum += sizeof(Record *);
    }
    return maxNum;
}