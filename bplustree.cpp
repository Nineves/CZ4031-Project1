#include "bplustree.h"

#include <iostream>
#include <cmath>

using namespace std;

BPTree::BPTree(size_t blockSize, Memory *dataMem, Memory *indexMem) {
    /*
        Initialize B+ Tree
    */
    root = nullptr;

    this->blockSize = blockSize;
    maxNumOfKeys = (new Node(blockSize)) -> getMaxKeyNum();
    numOfNodes = numOfLevels = 0;

    this->dataMem = dataMem;
    this->indexMem = indexMem;
}

void BPTree::insert(int key, Address *recordAddress) {
    /*
        Case 1. The BPTree is empty and does not have a root node.
    */
    if (root == nullptr){
        root = new Node(blockSize);
        root->isLeaf = true;
        root->curNumOfKeys += 1;
        root->parentAddr = nullptr;
        root->keys.push_back(key);

        //Todo: implement memWrite

        Address *LLNodeAddr = indexMem->memWrite(new LLNode(recordAddress, blockSize));
        root->pointers.push_back(LLNodeAddr);

        rootAddress = indexMem->memWrite(root, blockSize);

    }
    /*
        Case 2. The BPTree is not empty.
    */

    else{
        Node *curNode = root;
        Address *curNodeAddr = rootAddress;

        Node *parentNode = nullptr;
        Address *parentNodeAddr;

        Address *LLNodeAddr;

        /*
            Finding leaf nodes.
        */

        while (!curNode->isLeaf){
            parentNode = curNode;
            parentNodeAddr = curNodeAddr;
            for (int i = 0; i < curNode->maxNumOfKeys; i++) {
                if (curNode->keys[i] > key){
                    Node *nextNodeInDisk = indexMem->memRead(curNode->pointers[i], blockSize);
                    curNode = nextNodeInDisk;
                    
                    curNodeAddr = curNode->pointers[i];
                    break;
                }
                
                if (i == curNode -> maxNumOfKeys - 1){
                    Node *nextNodeInDisk = indexMem->memRead(curNode->pointers[i + 1], blockSize);
                    curNode = nextNodeInDisk;
                    curNodeAddr = curNode->pointers[i + 1];
                }
            }
            curNode->parentAddr = parentNodeAddr;
            
        }

        //We've found the leaf node
        //If there is still empty space for the new record, we insert the key and its pointer.
        
        if(curNode->curNumOfKeys < curNode->maxNumOfKeys){
            LLNodeAddr = indexMem->memWrite(new LLNode(recordAddress, blockSize));
            insertInNode(curNode, key, LLNodeAddr);
            indexMem->memWrite(curNode, curNodeAddr);
        }

        //If there is no empty space, we split the node into two nodes.
        else {
            int n = floor((curNode->maxNumOfKeys + 1) / 2);
            Node *splitLeafNode = new Node(blockSize);
            
            LLNodeAddr = indexMem->memWrite(new LLNode(recordAddress, blockSize));

            for (int i = n; i < curNode->curNumOfKeys; i++)
            {
                splitLeafNode->keys[i-n] = curNode->keys[i];
                splitLeafNode->pointers[i-n] = curNode->pointers[i];
            }

            splitLeafNode->curNumOfKeys = curNode->curNumOfKeys - n;

            for (int i = n; i < curNode->curNumOfKeys; i++)
            {
                curNode->keys[i] = -1;
                curNode->pointers[i] = nullptr;
            }

            //insert in the previous node
            if (curNode->keys[n-1] >= key)
            {
                insertInNode(curNode, key, LLNodeAddr);
            }

            //insert in the latter node
            else
            {
                insertInNode(splitLeafNode, key, LLNodeAddr);
            }
            

            Address *splitNodeAddr = indexMem->memWrite(splitLeafNode);

            curNode->pointers[curNode->maxNumOfKeys] = splitNodeAddr;
            indexMem->memWrite(curNode, curNodeAddr);

            //update parent nodes
            updateParents(splitLeafNode->keys[0], parentNodeAddr, splitNodeAddr);
        }

    }
    
}

void BPTree::insertInNode(Node *node, int key, Address *newNodeAddress) {
    /*
        Insert new key and pointer into a leaf node.
    */
    for(int i = 0; i < node->maxNumOfKeys; i++){
        if (node->keys[i] > key and i < node->maxNumOfKeys - 1 )
        {
            doShift(node, i);
            node->keys[i] = key;
            node->pointers[i] = newNodeAddress;
            node->curNumOfKeys ++;
            break;
        }

        else if (node->keys[i] == key)
        {
            LLNode *headNode = indexMem->memRead(node->pointers[i], blockSize);
            headNode->insert(newNodeAddress);
            break;
        }
        

    }
}

void BPTree::insertInnerNode(Node *curNode, int newKey, Address *childAddr) {
    /*
        Insert new key and pointer into a non-leaf node.
    */
     for (int i = 0; i < curNode->maxNumOfKeys; i++)
        {
            if (curNode->keys[i] > newKey and i < curNode->maxNumOfKeys - 1)
            {
                doInnerShift(curNode, i);
                curNode->keys[i] = newKey;
                curNode->pointers[i+1] = childAddr;
                curNode->curNumOfKeys ++;
                break;
            }

            if (i == curNode->maxNumOfKeys - 1)
            {
                curNode->keys[i] = newKey;
                curNode->pointers[i+1] = childAddr;
                curNode->curNumOfKeys ++;
                break;
            }
        }

}

void BPTree::doShift(Node *node, int start) {
    /*
    Shift both keys and pointers in leaf nodes
    */

    for(int j = node->curNumOfKeys - 1; j > start; j--){
        node->keys[j] = node->keys[j-1];
        node->pointers[j] = node->pointers[j-1];
    }
}

void BPTree::doInnerShift(Node *node, int start) {
    /*
    Shift both keys and pointers in non-leaf nodes
    */
   
    for(int j = node->curNumOfKeys - 1; j > start; j--){
        node->keys[j] = node->keys[j-1];
        node->pointers[j + 1] = node->pointers[j];
    }
}

void BPTree::updateParents(int newKey, Address *parentAddr, Address *childAddr) {

    Node *curNode = indexMem->memRead(parentAddr, blockSize);
    
    if (curNode->curNumOfKeys < curNode->maxNumOfKeys)
    {
       insertInnerNode(curNode, newKey, childAddr);
       indexMem->memWrite(curNode, parentAddr);
    }
    else
    {
        int n = floor((curNode->maxNumOfKeys + 1) / 2);
        Node *splitNonLeafNode = new Node(blockSize);
        splitNonLeafNode->parentAddr = parentAddr;

        for (int i = n; i < curNode->curNumOfKeys; i++)
        {
            splitNonLeafNode->keys[i-n] = curNode->keys[i];
            splitNonLeafNode->pointers[i-n + 1] = curNode->pointers[i];
        }

        splitNonLeafNode->pointers[0] = childAddr;

        splitNonLeafNode->curNumOfKeys = curNode->curNumOfKeys - n;

        for (int i = n; i < curNode->curNumOfKeys; i++)
        {
            curNode->keys[i] = -1;
            curNode->pointers[i + 1] = nullptr;
        }

        
        Address *splitNodeAddr = indexMem->memWrite(splitNonLeafNode);
        if (parentAddr == rootAddress)
        {
            Node *newRoot = new Node(blockSize);
            newRoot->keys.push_back(newKey);
            newRoot->curNumOfKeys = 1;

            newRoot->pointers.push_back(parentAddr);
            newRoot->pointers.push_back(splitNodeAddr);

            Address *newRootAddr =  indexMem->memWrite(newRoot);
            this->root = newRoot;
            this->rootAddress = newRootAddr;
        }
        else
        /*
        Recursive call.
        */
        {
            parentAddr = curNode->parentAddr;
            indexMem->memWrite(curNode, parentAddr);
            updateParents(splitNonLeafNode->keys[0], parentAddr, splitNodeAddr);
        }
        
    }
    
    
    

}