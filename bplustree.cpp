#include "bplustree.h"

#include <iostream>
#include <cmath>

using namespace std;

BPTree::BPTree(size_t blockSize) {
    /*
        Initialize B+ Tree
    */
    root = nullptr;

    this->blockSize = blockSize;
    maxNumOfKeys = (new Node(blockSize, true)) -> getMaxKeyNum();
    numOfNodes = numOfLevels = 0;

}

void BPTree::insert(int key, Record *recordAddress) {
    /*
        Case 1. The BPTree is empty and does not have a root node.
    */
    if (root == nullptr){
        root = new Node(blockSize, true);
        root->parentAddr = nullptr;
        root->insertLeafKey(key, recordAddress);
    }
    /*
        Case 2. The BPTree is not empty.
    */

    else{
        Node *curNode = root;

        Node *parentNode = nullptr;

        LLNode *LLNodeAddr;

        /*
            Finding leaf nodes.
        */

        while (!curNode->isLeaf){
            parentNode = curNode;
            for (int i = 0; i < curNode->maxNumOfKeys; i++) {
                if (curNode->keys[i] > key){

                    Node *nextNodeInDisk = curNode->ptrs.nodePointers[i];
                    curNode = nextNodeInDisk;                
                    break;
                }
                
                if (i == curNode -> maxNumOfKeys - 1){
                    Node *nextNodeInDisk = curNode->ptrs.nodePointers[i + 1];
                    curNode = nextNodeInDisk;
                }
            }
            curNode->parentAddr = parentNode;
            
        }

        //We've found the leaf node
        //If there is still empty space for the new record, we insert the key and its pointer.
        
        if(curNode->curNumOfKeys < curNode->maxNumOfKeys){
            int flag, preKey, curKey;
            preKey = curNode->keys[0];
            flag = curNode->insertLeafKey(key, recordAddress);
            if (flag == 1)  //need to update the key value in parent node
            {
                curKey = curNode->keys[0];
                curNode->parentAddr->updateKey(preKey, curKey);
            }
            
            
        }

        //If there is no empty space, we split the node into two nodes.
        else {
            int n = floor((curNode->maxNumOfKeys + 1) / 2);
            Node *splitLeafNode = new Node(blockSize, true);
            
            LLNodeAddr = new LLNode(blockSize);

            for (int i = n; i < curNode->curNumOfKeys; i++)
            {
                splitLeafNode->keys[i-n] = curNode->keys[i];
                splitLeafNode->ptrs.dataPointers[i-n] = curNode->ptrs.dataPointers[i];
            }

            splitLeafNode->curNumOfKeys = curNode->curNumOfKeys - n;

            for (int i = n; i < curNode->curNumOfKeys; i++)
            {
                curNode->keys[i] = -1;
                curNode->ptrs.dataPointers[i] = nullptr;
            }

            //insert in the previous node
            if (curNode->keys[n-1] >= key)
            {
                curNode->insertLeafKey(key, recordAddress);
            }

            //insert in the latter node
            else
            {
                splitLeafNode->insertLeafKey(key, recordAddress);
            }
            
            curNode->nextLeafNode = splitLeafNode;

            //update parent nodes
            updateParents(splitLeafNode->keys[0], parentNode, splitLeafNode);
        }

    }
    
}


void BPTree::updateParents(int key, Node *parentAddr, Node *childAddr) {

    Node *curNode = parentAddr;
    
    if (curNode->curNumOfKeys < curNode->maxNumOfKeys)
    {
        curNode->insertNonLeafKey(key, childAddr);
    }
    else
    {
        /*
            Splitting a node
        */
        Node* nodeToSplit = parentAddr;
        Node* insertNode = childAddr;
        int n = floor((nodeToSplit->maxNumOfKeys + 1) / 2);
        int pos = 0;
        int newKey = -1;
        Node *splitNonLeafNode = new Node(blockSize, false);
        splitNonLeafNode->parentAddr = nodeToSplit->parentAddr;

        for (int i = 0; i < nodeToSplit->curNumOfKeys; i++)
            {
                if (key < nodeToSplit->keys[i])
                {
                    pos = i;
                }
                else if (i = nodeToSplit->maxNumOfKeys - 1)
                {
                    pos = nodeToSplit-> maxNumOfKeys;
                }
            }
        if (pos != n)
        {
            for (int i = n; i < nodeToSplit->curNumOfKeys - 1; i++)
            {
                splitNonLeafNode->keys[i - n] = nodeToSplit->keys[i + 1];
            }

            for (int j = n; j < nodeToSplit->curNumOfKeys; j++)
            {
                splitNonLeafNode->ptrs.nodePointers[j - n] = nodeToSplit->ptrs.nodePointers[j + 1];
            }
            splitNonLeafNode->curNumOfKeys = nodeToSplit->maxNumOfKeys - n - 1;
            newKey = nodeToSplit->keys[n];
            
        }
        else //pos == n
        {
            for (int i = n; i < nodeToSplit->curNumOfKeys; i++)
            {
                splitNonLeafNode->keys[i - n] = nodeToSplit->keys[i];
                splitNonLeafNode->ptrs.nodePointers[i - n + 1] = nodeToSplit->ptrs.nodePointers[i + 1];
            }
            splitNonLeafNode->ptrs.nodePointers[0] = insertNode;
            splitNonLeafNode->curNumOfKeys = nodeToSplit->maxNumOfKeys - n;
            newKey = key;
        }
        

        for (int i = n; i < nodeToSplit->curNumOfKeys; i++)
            {
                nodeToSplit->keys[i] = -1;
                nodeToSplit->ptrs.nodePointers[i + 1] = nullptr;
            }
        
        nodeToSplit->curNumOfKeys = n;

        if (pos < n)
        {
            nodeToSplit->insertNonLeafKey(key, insertNode);
        }
        else if (pos > n)
        {
            splitNonLeafNode->insertNonLeafKey(key, insertNode);
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

            this->root = newRoot;
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