class Calculations 
{
    public:
        // Record
        int GetMaxNumOfRecordsPerBlock(int blockSize);
        int GetMaxNumOfRecordBlocks(int blockSize);
        int GetMaxSizeOfRecordBlocks(int blockSize);

        // Index
        int GetMaxNumOfKeysPerIndexBlock(int blockSize);
        int GetMinNumOfKeysPerInternalNode(int blockSize);
        int GetMinNumOfKeysPerLeafNode(int blockSize);
        int GetMaxHeightOfBPlusTree(int blockSize);
        int GetMaxNumOfIndexBlocks(int blockSize);
        int GetMaxSizeOfIndexBlocks(int blockSize);


        int GetTotalBlockSize(int blockSize);
};