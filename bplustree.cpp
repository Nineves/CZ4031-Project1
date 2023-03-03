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
        curNode->deleteLeafKey(key);
        // upDate parents
        if (curNode->keys[0] == key)
        {
            curNode->upDateDeletedParents(key);
            return;
        }
    }
    else
    {
        Node *lastNode = curNode->lastLeafNode;
        Node *nextNode = curNode->nextLeafNode;
        Node *siblingNode = nullptr;
        if (lastNode->curNumOfKeys > floor((curNode->maxNumOfKeys + 1) / 2) + 1)
        {
            // borrow one from left
            curNode->deleteLeafKey(key);
            int borrow = lastNode->keys[lastNode->curNumOfKeys];
            Node *borrowNode = lastNode->ptrs.dataPointers[lastNode->curNumOfKeys];
            lastNode->deleteLeafKey(borrow);
            curNode->insertLeafKey(borrow, borrowNode);
            return;
        }
        else if (nextNode->curNumOfKeys > floor((curNode->maxNumOfKeys + 1) / 2) + 1)
        {
            // borrow one from right
            curNode->deleteLeafKey(key);
            int borrow = nextNode->keys[0];
            Node *borrowNode = nextNode->ptrs.dataPointers[0];
            nextNode->deleteLeafKey(borrow);
            curNode->insertLeafKey(borrow, borrowNode);
            // upDate parents
            curNode->upDateDeletedParents(key);
            return;
        }
        else
        {
            // merge with right
            curNode->deleteLeafKey(key);
            // 要改
            merge(curNode, nextNode, -1);
            nextNode->parentAddr = nullptr;
            // 这块
            curNode = curNode->parentAddr;
            while (curNode->curNumOfKeys < floor((curNode->maxNumOfKeys) / 2))
            {
                parentNode = curNode->parentAddr;
                int flag = 0;
                // flag = 1 left
                // flag = -1 right
                // 还没写
                siblingNode = xxx;
                if (siblingNode->curNumOfKeys > floor((siblingNode->maxNumOfKeys + 1) / 2) + 1 && flag == 1)
                {
                    int borrow = siblingNode->keys[siblingNode->curNumOfKeys];
                    Node *borrowNode = siblingNode->ptrs.dataPointers[siblingNode->curNumOfKeys];
                    siblingNode->deleteLeafKey(borrow);
                    curNode->insertLeafKey(borrow, borrowNode);
                    return;
                }
                else if (siblingNode->curNumOfKeys > floor((siblingNode->maxNumOfKeys + 1) / 2) + 1 && flag == -1)
                {
                    int borrow = siblingNode->keys[0];
                    Node *borrowNode = siblingNode->ptrs.dataPointers[0];
                    siblingNode->deleteLeafKey(borrow);
                    curNode->insertLeafKey(borrow, borrowNode);
                    // update parents
                    curNode->upDateDeletedParents(key);
                    return;
                }
                else
                {
                    // 要改
                    merge(curNode, siblingNode, -1);
                    siblingNode->parentAddr = nullptr;
                    // 这块
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

void BPTree::updateParents(int key, Node *parentAddr, Node *childAddr)
{

    Node *curNode = parentAddr;

    // If there is empty space for insertion, we insert the node directly.
    if (curNode->curNumOfKeys < curNode->maxNumOfKeys)
    {
        curNode->insertNonLeafKey(key, childAddr);
        childAddr->parentAddr = parentAddr;
        return;
    }
    else
    {
        /*
            Splitting a non-leaf node
        */
        Node *nodeToSplit = parentAddr;
        Node *insertNode = childAddr;
        numOfNodes++;
        int n = floor((nodeToSplit->maxNumOfKeys) / 2);
        int pos = 0;
        int newKey = -1;
        Node *splitNonLeafNode = new Node(blockSize, false);
        splitNonLeafNode->parentAddr = nodeToSplit->parentAddr;

        if (key < nodeToSplit->keys[n] && key > nodeToSplit->keys[n - 1])
        {
            pos = n;
        }

        if (pos != n)
        {
            if (key < nodeToSplit->keys[n - 1])
            {
                for (int i = n; i < nodeToSplit->maxNumOfKeys; i++)
                {
                    splitNonLeafNode->keys[i - n] = nodeToSplit->keys[i];
                }
                splitNonLeafNode->curNumOfKeys = nodeToSplit->maxNumOfKeys - n;

                int k = 0;
                for (int j = n; j < nodeToSplit->maxNumOfKeys + 1; j++)
                {
                    splitNonLeafNode->ptrs.nodePointers[k] = nodeToSplit->ptrs.nodePointers[j];
                    k++;
                }

                newKey = nodeToSplit->keys[n - 1];
                for (int i = n - 1; i < nodeToSplit->maxNumOfKeys; i++)
                {
                    nodeToSplit->keys[i] = -1;
                    nodeToSplit->ptrs.nodePointers[i + 1] = nullptr;
                }
                nodeToSplit->curNumOfKeys = n - 1;
                nodeToSplit->insertNonLeafKey(key, childAddr);
            }
            else
            {
                int k = 0;
                for (int i = n + 1; i < nodeToSplit->maxNumOfKeys; i++)
                {
                    splitNonLeafNode->keys[k] = nodeToSplit->keys[i];
                    k++;
                }
                splitNonLeafNode->curNumOfKeys = nodeToSplit->maxNumOfKeys - n - 1;

                k = 0;
                for (int j = n + 1; j < nodeToSplit->maxNumOfKeys + 1; j++)
                {
                    splitNonLeafNode->ptrs.nodePointers[k] = nodeToSplit->ptrs.nodePointers[j];
                    k++;
                }
                newKey = nodeToSplit->keys[n];
                for (int i = n; i < nodeToSplit->maxNumOfKeys; i++)
                {
                    nodeToSplit->keys[i] = -1;
                    nodeToSplit->ptrs.nodePointers[i + 1] = nullptr;
                }
                nodeToSplit->curNumOfKeys = n;
                splitNonLeafNode->insertNonLeafKey(key, childAddr);
            }
        }
        else // pos == n
        {
            for (int i = n; i < nodeToSplit->maxNumOfKeys; i++)
            {
                splitNonLeafNode->keys[i - n] = nodeToSplit->keys[i];
            }

            int k = 1;
            for (int j = n + 1; j < nodeToSplit->maxNumOfKeys + 1; j++)
            {
                splitNonLeafNode->ptrs.nodePointers[k] = nodeToSplit->ptrs.nodePointers[j];
                k++;
            }

            splitNonLeafNode->ptrs.nodePointers[0] = childAddr;
            splitNonLeafNode->curNumOfKeys = nodeToSplit->maxNumOfKeys - n;
            newKey = key;
            childAddr->parentAddr = splitNonLeafNode;

            for (int i = n; i < nodeToSplit->maxNumOfKeys; i++)
            {
                nodeToSplit->keys[i] = -1;
                nodeToSplit->ptrs.nodePointers[i + 1] = nullptr;
            }
            nodeToSplit->curNumOfKeys = n;
        }

        for (int i = 0; i < splitNonLeafNode->curNumOfKeys + 1; i++)
        {
            // Update parent pointer in child nodes of split node
            splitNonLeafNode->ptrs.nodePointers[i]->parentAddr = splitNonLeafNode;
        }

        /*
            End of splitting node.
        */

        /*
            If the node to split is a root node, we need to create a new root node.
        */
        if (parentAddr == root)
        {
            Node *newRoot = new Node(blockSize, false);
            newRoot->ptrs.nodePointers[0] = parentAddr;
            newRoot->insertNonLeafKey(newKey, splitNonLeafNode);
            numOfLevels++;
            numOfNodes++;

            this->root = newRoot;
            parentAddr->parentAddr = newRoot;

            return;
        }
        else

        /*
        Recursive call.
        */
        {
            parentAddr = curNode->parentAddr;

            updateParents(newKey, parentAddr, splitNonLeafNode);
        }
    }
}

void BPTree::updateDeletedParents(int key, Node *parentAddr, Node *childAddr)
{
}