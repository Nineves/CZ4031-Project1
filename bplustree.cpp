#include "bplustree.h"

#include <iostream>
#include <cmath>

using namespace std;

BPTree::BPTree(unsigned int blockSize)
{
    /*
        Initialize B+ Tree
    */
    root = nullptr;

    this->blockSize = blockSize;
    this->leafHead = nullptr;
    maxNumOfKeys = (new Node(blockSize, true))->getMaxKeyNum();
    numOfNodes = numOfLevels = 0;
}

void BPTree::printNode(Node *treenode)
{
    cout << treenode->ptrs.nodePointers[0] << "->";
    for (int i = 0; i < treenode->curNumOfKeys; i++)
    {
        cout << "[" << treenode->keys[i] << "]"
             << "->";
        if (i + 1 == treenode->curNumOfKeys)
        {
            cout << treenode->ptrs.nodePointers[i + 1] << endl;
            break;
        }
        cout << treenode->ptrs.nodePointers[i + 1] << "->";
    }
}

bool BPTree::CheckTree()
{
    Node *pThisNode = leafHead;
    Node *pNextNode = nullptr;
    while (nullptr != pThisNode)
    {
        pNextNode = pThisNode->nextLeafNode;
        if (nullptr != pNextNode)
        {
            if (pThisNode->keys[pThisNode->curNumOfKeys - 1] > pNextNode->keys[0])
            {
                return false;
            }
        }
        pThisNode = pNextNode;
    }

    return CheckNode(getRoot());
}

bool BPTree::CheckNode(Node *pNode)
{
    if (nullptr == pNode)
    {
        return true;
    }

    if (pNode->isLeaf)
    {
        return true;
    }

    int i = 0;
    bool ret = false;

    if ((pNode->curNumOfKeys < floor(pNode->maxNumOfKeys / 2)) && (pNode != getRoot()))
    {
        cout << "Key number too low." << endl;
        return false;
    }

    for (i = 0; i < pNode->curNumOfKeys - 1; i++)
    {

        if ((pNode->keys[i] > pNode->keys[i + 1]) && (pNode->curNumOfKeys > 1))
        {
            cout << "Sequence false" << endl;
            printNode(pNode);
            return false;
        }
    }

    for (int i = 0; i < pNode->curNumOfKeys; i++)
    {
        Node *childNode = pNode->ptrs.nodePointers[i + 1];
        int curKey = pNode->keys[i];

        if (childNode->keys[0] < curKey)
        {
            cout << "Key incorrect " << curKey << endl;
            printNode(pNode);
            cout << "\n index " << i << endl;
            cout << childNode << endl;
            printNode(childNode);
            return false;
        }

        if (childNode->parentAddr != pNode)
        {
            cout << "Parent incorrect" << endl;
            return false;
        }
    }

    for (i = 0; i <= pNode->curNumOfKeys; i++)
    {
        ret = CheckNode(pNode->ptrs.nodePointers[i]);
        if (false == ret)
        {
            return false;
        }
    }

    return true;
}

void BPTree::insert(int key, Record *recordAddress)
{
    /*
        Case 1. The BPTree is empty and does not have a root node.
    */
    if (root == nullptr)
    {
        root = new Node(blockSize, true);
        root->parentAddr = nullptr;
        root->insertLeafKey(key, recordAddress);
        leafHead = root;
        numOfNodes = 1;
        numOfLevels = 1;
    }
    /*
        Case 2. The BPTree is not empty.
    */

    else
    {
        Node *curNode = root;
        Node *parentNode = nullptr;
        LLNode *LLNodeAddr;

        // Traverse down the tree to leaf node
        curNode = searchInsertLeafNode(key);
        parentNode = curNode->parentAddr;

        // We've found the leaf node
        // If there is still empty space for the new record, we insert the key and its pointer.

        if (curNode->curNumOfKeys < curNode->maxNumOfKeys)
        {
            int flag, preKey, curKey;
            preKey = curNode->keys[0];
            curNode->insertLeafKey(key, recordAddress);
        }

        // If there is no empty space, we split the leaf node into two nodes.
        else
        {
            int n = floor((curNode->maxNumOfKeys + 1) / 2);
            Node *splitLeafNode = new Node(blockSize, true);
            numOfNodes++;

            LLNodeAddr = new LLNode(blockSize);

            for (int i = n; i < curNode->curNumOfKeys; i++)
            {
                splitLeafNode->keys[i - n] = curNode->keys[i];
                splitLeafNode->ptrs.dataPointers[i - n] = curNode->ptrs.dataPointers[i];
            }

            splitLeafNode->curNumOfKeys = curNode->curNumOfKeys - n;

            for (int i = n; i < curNode->curNumOfKeys; i++)
            {
                curNode->keys[i] = -1;
                curNode->ptrs.dataPointers[i] = nullptr;
            }

            curNode->curNumOfKeys = n;

            // insert in the previous node
            if (curNode->keys[n - 1] >= key)
            {
                curNode->insertLeafKey(key, recordAddress);
            }

            // insert in the latter node
            else
            {
                splitLeafNode->insertLeafKey(key, recordAddress);
            }

            curNode->nextLeafNode = splitLeafNode;
            splitLeafNode->lastLeafNode = curNode;

            if (numOfLevels == 1) // CurNode is both leaf node and root node
            {
                createNewRoot(curNode, splitLeafNode);
            }
            else
            {
                // update parent nodes
                updateParents(splitLeafNode->keys[0], parentNode, splitLeafNode);
            }
        }
    }
}

void BPTree::remove(int key)
{
    Node *curNode = root;
    Node *parentNode = nullptr;

    curNode = searchDeleteLeafNode(key);
    parentNode = curNode->parentAddr;

    /*
    1. Easy case
    */
    if (curNode->curNumOfKeys > floor((curNode->maxNumOfKeys + 1) / 2) + 1)
    {
        int first = curNode->keys[0];
        curNode->deleteLeafKey(key);
        // upDate parents
        if (key == first)
        {
            curNode->updateDeletedParents(key);
            return;
        }
    }
    else
    {
        Node *lastNode = nullptr;
        Node *nextNode = nullptr;
        if (curNode->lastLeafNode->parentAddr = curNode->parentAddr)
        {
            Node *lastNode = curNode->lastLeafNode;
        }
        if (curNode->nextLeafNode->parentAddr = curNode->parentAddr)
        {
            Node *nextNode = curNode->nextLeafNode;
        }
        Node *siblingNode = nullptr;
        if (lastNode != nullptr && lastNode->curNumOfKeys >= floor((curNode->maxNumOfKeys + 1) / 2) + 1)
        {
            // borrow one from left
            int flag = 0;
            if (curNode->keys[0] == key)
            {
                flag = 1;
            }
            curNode->deleteLeafKey(key);
            int borrow = lastNode->keys[lastNode->curNumOfKeys];
            LLNode *borrowNode = lastNode->ptrs.dataPointers[lastNode->curNumOfKeys];
            lastNode->deleteLeafKey(borrow);
            curNode->moveLeafKey(0, borrow, borrowNode);
            if (flag == 1)
            {
                curNode->updateDeletedParents(key);
            }
            return;
        }
        else if (nextNode != nullptr && nextNode->curNumOfKeys >= floor((curNode->maxNumOfKeys + 1) / 2) + 1)
        {
            // borrow one from right
            int flag = 0;
            if (curNode->keys[0] == key)
            {
                flag = 1;
            }
            curNode->deleteLeafKey(key);
            int borrow = nextNode->keys[0];
            LLNode *borrowNode = nextNode->ptrs.dataPointers[0];
            nextNode->deleteLeafKey(borrow);
            curNode->moveLeafKey(curNode->curNumOfKeys, borrow, borrowNode);
            nextNode->updateDeletedParents(borrow);
            if (flag == 1)
            {
                curNode->updateDeletedParents(key);
            }
            return;
        }
        else
        {
            // merge with right
            curNode->deleteLeafKey(key);
            if (nextNode != nullptr)
            {
                mergeLeaf(curNode, nextNode);
                curNode->updateDeletedParents(key);
            }
            else
            {
                mergeLeaf(lastNode, curNode);
                parentNode = lastNode->parentAddr;
                curNode = lastNode;
                curNode->parentAddr = parentNode;
                curNode->updateDeletedParents(key);
            }
            curNode = curNode->parentAddr;
            curNode->curNumOfKeys -= 1;
            // shift curNode
            while (curNode->curNumOfKeys < floor((curNode->maxNumOfKeys) / 2))
            {
                parentNode = curNode->parentAddr;
                int flag = 0;
                for (int i = 0; i <= parentNode->curNumOfKeys; i++)
                {
                    if (parentNode->ptrs.nodePointers[i] == curNode)
                    {
                        if (i == parentNode->curNumOfKeys)
                        {
                            siblingNode = parentNode->ptrs.nodePointers[i - 1];
                            flag = 1;
                            break;
                        }
                        else
                        {
                            siblingNode = parentNode->ptrs.nodePointers[i + 1];
                            flag = -1;
                            break;
                        }
                    }
                }
                //borrow from left possible
                if (siblingNode->curNumOfKeys > floor((siblingNode->maxNumOfKeys + 1) / 2) + 1 && flag == 1)
                {
                    int flag = 0;
                    if (curNode->keys[0] == key)
                    {
                        flag = 1;
                    }
                    int borrow = siblingNode->keys[siblingNode->curNumOfKeys];
                    LLNode *borrowNode = siblingNode->ptrs.dataPointers[siblingNode->curNumOfKeys];
                    siblingNode->deleteLeafKey(borrow);
                    curNode->moveLeafKey(0, borrow, borrowNode);
                    if (flag == 1)
                    {
                        curNode->updateDeletedParents(key);
                    }
                    return;
                }

                //borrow from right impossible
                else if (siblingNode->curNumOfKeys > floor((siblingNode->maxNumOfKeys + 1) / 2) + 1 && flag == -1)
                {
                    int flag = 0;
                    if (curNode->keys[0] == key)
                    {
                        flag = 1;
                    }
                    int borrow = siblingNode->keys[0];
                    LLNode *borrowNode = siblingNode->ptrs.dataPointers[0];
                    siblingNode->deleteLeafKey(borrow);
                    curNode->moveLeafKey(curNode->curNumOfKeys, borrow, borrowNode);
                    nextNode->updateDeletedParents(borrow);
                    if (flag == 1)
                    {
                        curNode->updateDeletedParents(key);
                    }
                    return;
                }
                else
                {
                    //borrow from left, but not possible, needs to merge with left
                    if (flag == 1)
                    {
                        mergeNonLeaf(siblingNode, curNode);
                        parentNode = siblingNode->parentAddr;
                        curNode->parentAddr = nullptr;
                        curNode = siblingNode;
                        curNode->parentAddr = parentNode;
                    }
                    //borrow from right, but not possible, needs to merge with right
                    else if (flag == -1)
                    {
                        mergeNonLeaf(curNode, siblingNode);
                        siblingNode->parentAddr = nullptr;
                    }
                    curNode = curNode->parentAddr;
                    if (curNode->curNumOfKeys < floor((curNode->maxNumOfKeys) / 2) && curNode->parentAddr == root)
                    {
                        curNode->parentAddr = nullptr;
                        root = curNode;
                        return;
                    }
                }
            }
        }
    }
}


void BPTree::createNewRoot(Node *node1, Node *node2)
{
    Node *newRoot = new Node(blockSize, false);
    newRoot->ptrs.nodePointers[0] = node1;
    newRoot->ptrs.nodePointers[1] = node2;
    newRoot->keys[0] = node2->keys[0];
    newRoot->curNumOfKeys = 1;
    numOfLevels++;
    numOfNodes++;

    node1->parentAddr = newRoot;
    node2->parentAddr = newRoot;
    this->root = newRoot;
}

Node *BPTree::searchInsertLeafNode(int key)
{
    Node *curNode = root;

    Node *parentNode = nullptr;
    Node *fordebug;
    int numOfPtrs;

    /*
        Finding leaf nodes.
    */

    while (!curNode->isLeaf)
    {
        parentNode = curNode;
        for (int i = 0; i < curNode->curNumOfKeys; i++)
        {
            if (curNode->keys[i] > key)
            {
                fordebug = curNode;
                curNode = curNode->ptrs.nodePointers[i];
                break;
            }

            if (i == curNode->curNumOfKeys - 1)
            {
                fordebug = curNode;
                curNode = curNode->ptrs.nodePointers[i + 1];
                break;
            }
        }
        curNode->parentAddr = parentNode;
    }

    return curNode;
}

Node *BPTree::searchDeleteLeafNode(int key)
{
    Node *curNode = root;

    Node *parentNode = nullptr;
    Node *fordebug;
    int numOfPtrs;
    int flag = 0;

    /*
        Finding leaf nodes.
    */

    while (!curNode->isLeaf)
    {
        parentNode = curNode;
        for (int i = 0; i < curNode->curNumOfKeys; i++)
        {
            if (flag == 0)
            {
                if (curNode->keys[i] == key)
                {
                    fordebug = curNode;
                    curNode = curNode->ptrs.nodePointers[i];
                    flag = 1;
                    break;
                }

                if (i == curNode->curNumOfKeys - 1)
                {
                    fordebug = curNode;
                    curNode = curNode->ptrs.nodePointers[i + 1];
                    break;
                }
            }
        }
        curNode->parentAddr = parentNode;
    }

    if (flag == 0)
    {
        throw("Cannot find the key to be deleted!");
    }

    return curNode;
}

Node *BPTree::searchLeafNode(int key)
{
    if (root == nullptr)
    {
        cout << "Tree is empty\n";
    }
    else
    {
        Node *curNode = root;
        while (!curNode->isLeaf)
        {
            for (int i = 0; i < curNode->curNumOfKeys; i++)
            {
                if (curNode->keys[i] > key)
                {
                    curNode = curNode->ptrs.nodePointers[i];
                    // numOfAccessNodes++;
                    break;
                }
                if (i == curNode->curNumOfKeys - 1)
                {
                    curNode = curNode->ptrs.nodePointers[i + 1];
                    break;
                }
            }
        }
        for (int i = 0; i < curNode->curNumOfKeys; i++)
        {
            if (curNode->keys[i] == key)
            {
                cout << "We found the key!\n";
                return curNode;
            }
        }
        cout << "We cannot find the key!\n";
    }
    return nullptr;
}

int BPTree::getNumOfNodeSearch(int key)
{
    if (root == nullptr)
    {
        cout << "Tree is empty\n";
    }
    else
    {
        Node *curNode = root;
        int numOfAccessNodes = 1;
        while (!curNode->isLeaf)
        {
            for (int i = 0; i < curNode->curNumOfKeys; i++)
            {
                if (curNode->keys[i] > key)
                {
                    curNode = curNode->ptrs.nodePointers[i];
                    numOfAccessNodes++;
                    break;
                }
                if (i == curNode->curNumOfKeys - 1)
                {
                    curNode = curNode->ptrs.nodePointers[i + 1];
                    numOfAccessNodes++;
                    break;
                }
            }
        }
        cout << "Number of nodes accessed in searching process: " << numOfAccessNodes << endl;
        return numOfAccessNodes;
    }
    return 0;
}


void BPTree::mergeLeaf(Node *sourceNode, Node *mergeNode)
{
    for (int j = 0; j < mergeNode->curNumOfKeys; j++)
    {
        sourceNode->keys[sourceNode->curNumOfKeys + j] = mergeNode->keys[j];
        sourceNode->ptrs.nodePointers[sourceNode->curNumOfKeys + j] = mergeNode->ptrs.nodePointers[j];
        mergeNode->ptrs.nodePointers[j] = nullptr;
    }
    sourceNode->curNumOfKeys = sourceNode->curNumOfKeys + mergeNode->curNumOfKeys;
    sourceNode->ptrs.nodePointers[sourceNode->maxNumOfKeys] = sourceNode->nextLeafNode;
    Node *parentNode = sourceNode->parentAddr;
    for (int i = 0; i <= parentNode->curNumOfKeys; i++)
    {
        if (parentNode->ptrs.nodePointers[i] == mergeNode)
        {
            for (int k = i; k <= parentNode->curNumOfKeys; k++)
            {
                parentNode->ptrs.nodePointers[k] = parentNode->ptrs.nodePointers[k + 1];
                parentNode->keys[k] = parentNode->keys[k + 1];
            }
            break;
        }
    }
    parentNode->curNumOfKeys -= 1;
    return;
}

void BPTree::mergeNonLeaf(Node *sourceNode, Node *mergeNode)
{
    int addKey = 0;
    while (!mergeNode->isLeaf)
    {
        mergeNode = mergeNode->ptrs.nodePointers[0];
    }
    addKey = mergeNode->keys[0];
    for (int j = 0; j < mergeNode->curNumOfKeys + 1; j++)
    {
        sourceNode->keys[sourceNode->curNumOfKeys + j] = mergeNode->keys[j];
        sourceNode->ptrs.nodePointers[sourceNode->curNumOfKeys + j] = mergeNode->ptrs.nodePointers[j];
        mergeNode->ptrs.nodePointers[j] = nullptr;
    }
    sourceNode->curNumOfKeys = sourceNode->curNumOfKeys + mergeNode->curNumOfKeys;
    sourceNode->ptrs.nodePointers[sourceNode->maxNumOfKeys] = sourceNode->nextLeafNode;
    Node *parentNode = sourceNode->parentAddr;
    for (int i = 0; i <= parentNode->curNumOfKeys; i++)
    {
        if (parentNode->ptrs.nodePointers[i] == mergeNode)
        {
            for (int k = i; k <= parentNode->curNumOfKeys; k++)
            {
                parentNode->ptrs.nodePointers[k] = parentNode->ptrs.nodePointers[k + 1];
            }

            parentNode->keys[i] = addKey;

            for (int k = i + 1; k <= parentNode->curNumOfKeys; k++)
            {
                parentNode->keys[k] = parentNode->keys[k + 1];
            }
            break;
        }
    }
    parentNode->curNumOfKeys -= 1;
    return;
}