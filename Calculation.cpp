#include <cmath>
#include <iostream>
#include "Calculation.h"
#include "constants.h"

int Calculation::GetMaxNumOfRecordsPerBlock(int blockSize)
{
    int numOfRecords = 1;
    // 8 is the number of bits in 1 byte
    while ((numOfRecords * RECORD_SIZE) <= blockSize)
    {
        numOfRecords++;
    }

    return numOfRecords - 1;
}

int Calculation::GetMaxNumOfRecordBlocks(int blockSize)
{
    int numOfRecords = GetMaxNumOfRecordsPerBlock(blockSize);
    int numOfBlocks = ceil(TOTAL_NUM_OF_RECORDS / numOfRecords);

    return numOfBlocks;
}

int Calculation::GetMaxNumOfKeysPerIndexBlock(int blockSize)
{
    int numOfKeys = 1;
    while ((numOfKeys * INDEX_KEY_SIZE) + ((numOfKeys + 1) * INDEX_POINTER_SIZE) <= blockSize)
    {
        numOfKeys++;
    }

    return numOfKeys - 1;
}

int Calculation::GetMinNumOfKeysPerInternalNode(int blockSize)
{
    int maxNumOfKeys = GetMaxNumOfKeysPerIndexBlock(blockSize);
    int minNumOfKeys = floor(maxNumOfKeys / 2);

    return minNumOfKeys;
}

int Calculation::GetMinNumOfKeysPerLeafNode(int blockSize)
{
    int maxNumOfKeys = GetMaxNumOfKeysPerIndexBlock(blockSize);
    int minNumOfKeys = floor((maxNumOfKeys + 1) / 2);

    return minNumOfKeys;
}

int Calculation::GetMaxHeightOfBPlusTree(int blockSize)
{
    int minNumOfKeysPerInternalNode = GetMinNumOfKeysPerInternalNode(blockSize);
    int minNumOfKeysPerLeafNode = GetMinNumOfKeysPerLeafNode(blockSize);
    int height = 1;
    while ((2 * pow(minNumOfKeysPerInternalNode + 1, height - 2) * minNumOfKeysPerLeafNode) <= TOTAL_NUM_OF_RECORDS)
    {
        height++;
    }

    return height - 1;
}

int Calculation::GetMaxNumOfIndexBlocks(int blockSize)
{
    int maxHeight = GetMaxHeightOfBPlusTree(blockSize);
    int minNumOfKeysPerInternalNode = GetMinNumOfKeysPerInternalNode(blockSize);
    int numOfBlocks = 1;
    for (int i = 2; i <= maxHeight; i++)
    {
        numOfBlocks += 2 * pow(minNumOfKeysPerInternalNode + 1, i - 2);
    }

    return numOfBlocks;
}

int Calculation::GetMaxSizeOfRecordBlocks(int blockSize)
{
    int numOfBlocks = GetMaxNumOfRecordBlocks(blockSize);
    int sizeOfBlocks = numOfBlocks * blockSize;

    return sizeOfBlocks;
}

int Calculation::GetMaxSizeOfIndexBlocks(int blockSize)
{
    int numOfBlocks = GetMaxNumOfIndexBlocks(blockSize);
    int sizeOfBlocks = numOfBlocks * blockSize;

    return sizeOfBlocks;
}

int Calculation::GetTotalBlockSize(int blockSize)
{
    int sizeOfRecordBlocks = GetMaxSizeOfRecordBlocks(blockSize);
    int sizeOfIndexBlocks = GetMaxSizeOfIndexBlocks(blockSize);
    int totalSize = sizeOfRecordBlocks + sizeOfIndexBlocks;

    return totalSize;
}
