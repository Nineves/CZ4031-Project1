#include "bplustree.h"

#include <iostream>
#include <cmath>

using namespace std;

BPTree::BPTree(unsigned int blockSize) {
    /*
        Initialize B+ Tree
    */
    root = nullptr;

    this->blockSize = blockSize;
    maxNumOfKeys = (new Node(blockSize, true)) -> getMaxKeyNum();
    numOfNodes = numOfLevels = 0;

}

void BPTree::printNode(Node* treenode){
    cout<<treenode->ptrs.nodePointers[0]<<"->";
    for (int i = 0; i < treenode->curNumOfKeys; i++)
    {
        cout<<"["<<treenode->keys[i]<<"]"<<"->";
        if (i + 1 == treenode->curNumOfKeys)
        {
             cout<<treenode->ptrs.nodePointers[i + 1]<<endl;
             break;
        }
        cout<<treenode->ptrs.nodePointers[i + 1]<<"->";
    }
}

void BPTree::insert(int key, Record *recordAddress) {
    /*
        Case 1. The BPTree is empty and does not have a root node.
    */
    if (root == nullptr){
        root = new Node(blockSize, true);
        root->parentAddr = nullptr;
        root->insertLeafKey(key, recordAddress);
        numOfNodes = 1;
        numOfLevels = 1;
    }
    /*
        Case 2. The BPTree is not empty.
    */

    else{
        Node *curNode = root;


        Node *parentNode = nullptr;

        LLNode *LLNodeAddr;

        //Traverse down the tree to leaf node

        curNode = searchLeafNode(key);
        parentNode = curNode->parentAddr;

        //We've found the leaf node
        //If there is still empty space for the new record, we insert the key and its pointer.
        
        if(curNode->curNumOfKeys < curNode->maxNumOfKeys){
            int flag, preKey, curKey;
            preKey = curNode->keys[0];
            curNode->insertLeafKey(key, recordAddress);                 
        }

        //If there is no empty space, we split the leaf node into two nodes.
        else {
            int n = floor((curNode->maxNumOfKeys + 1) / 2);
            Node *splitLeafNode = new Node(blockSize, true);
            numOfNodes ++;
            
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

            curNode->curNumOfKeys = n;

            //insert in the previous node
            if (curNode->keys[n - 1] >= key)
            {
                curNode->insertLeafKey(key, recordAddress);
            }

            //insert in the latter node
            else
            {
                splitLeafNode->insertLeafKey(key, recordAddress);
            }
            
            curNode->nextLeafNode = splitLeafNode;

            if (numOfLevels == 1) //CurNode is both leaf node and root node
            {
                createNewRoot(curNode, splitLeafNode);
            }
            else
            {
                //update parent nodes
                updateParents(splitLeafNode->keys[0], parentNode, splitLeafNode);
            }
            
          
        }

    }
    
}

void BPTree::createNewRoot(Node *node1, Node *node2) {
    Node *newRoot = new Node(blockSize, false);
    newRoot->ptrs.nodePointers[0] = node1;
    newRoot->ptrs.nodePointers[1] = node2;
    newRoot->keys[0] = node2->keys[0];
    newRoot->curNumOfKeys = 1;
    numOfLevels ++;
    numOfNodes ++;
    
    node1->parentAddr = newRoot;
    node2->parentAddr = newRoot;
    this->root = newRoot;
}

Node* BPTree::searchLeafNode(int key) {
    Node *curNode = root;

    Node *parentNode = nullptr;
    Node* fordebug;
    int numOfPtrs;

        /*
            Finding leaf nodes.
        */

        while (!curNode->isLeaf){
            parentNode = curNode;
            for (int i = 0; i < curNode->curNumOfKeys; i++) {
                if (curNode->keys[i] > key) {
                    fordebug = curNode;
                    curNode = curNode->ptrs.nodePointers[i];                
                    break;
                }
                
                if (i == curNode -> curNumOfKeys - 1) { 
                    fordebug = curNode;
                    curNode = curNode->ptrs.nodePointers[i + 1];
                    break;
                }
            }
            curNode->parentAddr = parentNode;           
        }

    return curNode;
}


void BPTree::updateParents(int key, Node *parentAddr, Node *childAddr) {

    Node *curNode = parentAddr;
    
    //If there is empty space for insertion, we insert the node directly.
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
        Node* nodeToSplit = parentAddr;
        Node* insertNode = childAddr;
        numOfNodes ++;
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
            childAddr->parentAddr = splitNonLeafNode;
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
            childAddr->parentAddr = parentAddr;
        }
        else if (pos > n)
        {
            splitNonLeafNode->insertNonLeafKey(key, insertNode);
            childAddr->parentAddr = splitNonLeafNode;
        }

        for (int i = 0; i < splitNonLeafNode->curNumOfKeys + 1; i++)
        {
            //Update parent pointer in child nodes of split node
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
            numOfLevels ++;
            numOfNodes ++;

            this->root = newRoot;
            parentAddr->parentAddr = newRoot;
            childAddr->parentAddr = newRoot;

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