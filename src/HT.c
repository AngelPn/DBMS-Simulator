#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "../headers/HT.h"
#include "../headers/BF.h"

#define NEXT_BUCKET BLOCK_SIZE-sizeof(int)-1
#define NEXT        BLOCK_SIZE-2*sizeof(int)-1
#define REC_NUM     BLOCK_SIZE-sizeof(int)-1

int HT_CreateIndex( char *fileName, char attrType, char* attrName,int attrLength, int buckets ) 
{
    BF_Init();
    // Create file in block - level
    if (BF_CreateFile(fileName) < 0) {
		BF_PrintError("Error creating file");
		exit(EXIT_FAILURE);
	}

    // Open the created file in block - level and get the file identifier
    int fileDesc;
    if (fileDesc = BF_OpenFile(fileName) < 0){
        BF_PrintError("Error opening file");
        exit(EXIT_FAILURE);
    }

    // Allocate the header block
    if (BF_AllocateBlock(fileDesc) < 0){
        BF_PrintError("Error allocating block");
        exit(EXIT_FAILURE);
    }

    void *block = NULL;
    if (BF_ReadBlock(fileDesc, BF_GetBlockCounter(fileDesc) - 1, &block) < 0){
        BF_PrintError("Error reading block");
        exit(EXIT_FAILURE);
    }
    HT_info info = {.fileDesc = fileDesc,
                    .attrType = attrType, 
                    .attrName = malloc(sizeof(char)*(strlen(attrName)+1)),
                    .attrLength = attrLength,
                    .numBuckets = buckets,
                    .header_block_ID = BF_GetBlockCounter(fileDesc) - 1
                    };
    strcpy(info.attrName, attrName);
    memcpy(block, &info, sizeof(HT_info));
    //free(info.attrName);
    
    if (BF_WriteBlock(fileDesc, 0) < 0)
        return -1;

    int n_blocks = buckets*sizeof(int)/(BLOCK_SIZE-sizeof(int));
    int remainder = (buckets*sizeof(int))%(BLOCK_SIZE-sizeof(int));
   
    void *curr_block = NULL;
    void *prev_block = block;
    int blockID;
    int prev_blockID = BF_GetBlockCounter(fileDesc) - 1;
    
    for (int i = 1; i <= n_blocks; i++)
    {    
        if (BF_AllocateBlock(fileDesc) < 0){
            BF_PrintError("Error allocating block");
            exit(EXIT_FAILURE);
        }
        blockID = BF_GetBlockCounter(fileDesc)-1;
        if (BF_ReadBlock(fileDesc, blockID, &curr_block) < 0){
            BF_PrintError("Error reading block");
            exit(EXIT_FAILURE);
        }

        memcpy(prev_block + NEXT_BUCKET, &blockID, sizeof(int));
        if (BF_WriteBlock(fileDesc, prev_blockID) < 0)
            return -1;

        int count = -1;
        for (int j = 0; j < (BLOCK_SIZE/sizeof(int)-1); j++) {
            memcpy(curr_block+j*sizeof(int), &count, sizeof(int));
        }
        if (BF_WriteBlock(fileDesc,blockID) < 0)
            return -1;

        prev_block = curr_block;
        prev_blockID = blockID;
    }
    if (remainder > 0){
        int j;
        if (BF_AllocateBlock(fileDesc) < 0){
            BF_PrintError("Error allocating block");
            exit(EXIT_FAILURE);
        }
        blockID = BF_GetBlockCounter(fileDesc)-1;
        if (BF_ReadBlock(fileDesc, blockID , &curr_block) < 0){
            BF_PrintError("Error reading block");
            exit(EXIT_FAILURE);
        }
        memcpy(prev_block + NEXT_BUCKET, &blockID, sizeof(int));
        if (BF_WriteBlock(fileDesc, prev_blockID) < 0)
            return -1;

        int count = -1;
        for (j = 0; j < remainder; j++)
            memcpy(curr_block+j*sizeof(int), &count, sizeof(int));
        }
        if (BF_WriteBlock(fileDesc, 0) < 0)
            return -1;
    }
    // printf("%d\n", BF_GetBlockCounter(fileDesc));
    return 0;
}

HT_info *HT_OpenIndex(char *fileName){
    int fileDesc = 0;
    if (fileDesc = BF_OpenFile(fileName) < 0){
        BF_PrintError("Error opening file");
		exit(EXIT_FAILURE);
    }
    void *header_block = NULL;
    if (BF_ReadBlock(fileDesc, 0, &header_block) < 0){
        BF_PrintError("Error reading block");
        exit(EXIT_FAILURE);
    }
    HT_info *header_info = (HT_info *)header_block;

    HT_info *info = (HT_info *)malloc(sizeof(HT_info));
    printf("header_info->attrName = %s\n", header_info->attrName);
    
    memcpy(info, header_block, sizeof(HT_info));

    return info;
}

int HT_CloseIndex(HT_info *header_info){
    if (BF_CloseFile(header_info->fileDesc) < 0){
        BF_PrintError("Error closing file");
        return -1;
    }
    free(header_info->attrName);
    free(header_info);
    return 0;
}

int hash(long int nbuckets, void *key){
    int k = *(int *)key;
    int index;
    
    index = k%nbuckets;

    // double c = (sqrt(5) - 1)/2;
    // index = floor(nbuckets*(k*(int)c%1));

    return index;
}

int HT_InsertEntry(HT_info header_info, Record record){

    void *key = get_key(record, header_info.attrName);
    int index = hash(header_info.numBuckets, key);
    printf("index = %d\n", index);

    int nbuckets_in_block = BLOCK_SIZE/sizeof(int) - 1; //127
    int blockID_bucket = index/nbuckets_in_block + 1; //+1 because of header block
    printf("blockID_bucket = %d\n", blockID_bucket);

    void *bucket_block = NULL;
    if (BF_ReadBlock(header_info.fileDesc, blockID_bucket, &bucket_block) < 0){
        BF_PrintError("Error reading block");
        return -1;
    }

    int blockID = *(int *)(bucket_block + sizeof(int)*abs((index-1) - nbuckets_in_block*(blockID_bucket-1)) -1);
    printf("blockID = %d\n", blockID);
 
    int block_susID = -1;
    int count = 0;
    int prev_blockID = blockID_bucket;
    void *current_block;

    while ( blockID != -1) { //while (current_block != NULL)

        if (BF_ReadBlock(header_info.fileDesc, blockID, &current_block) < 0){
            BF_PrintError("Error reading block");
            return -1;
        }

        count = *(int *)(current_block + REC_NUM);

        for (int j = 0; j < count; j++){

            Record current_rec = (Record)(current_block + j*RECORD_SIZE);

            void *record_key = get_key(record, header_info.attrName);
            void *current_key = get_key(current_rec, header_info.attrName);
            //printf("keys:%d-%d\t", *(int *)record_key, *(int *)current_key);
            
            if (memcmp(record_key, current_key, header_info.attrLength) == 0){
                return -1;
            }
        }
        if (count < BLOCK_SIZE/RECORD_SIZE){
            block_susID = blockID;
        }
        prev_blockID = blockID;
        blockID = *(int *)(current_block + NEXT);
    }

    if (block_susID == -1){ /*if all previous blocks are full*/
        if (BF_AllocateBlock(header_info.fileDesc) < 0){
            BF_PrintError("Error allocating block");
            return -1;
        }
        void *block;
        blockID = BF_GetBlockCounter(header_info.fileDesc)-1;
        if (BF_ReadBlock(header_info.fileDesc, blockID, &block) < 0){
            BF_PrintError("Error reading block");
            return -1;
        }
        void *prev_block;
        if (BF_ReadBlock(header_info.fileDesc, prev_blockID, &prev_block) < 0){
            BF_PrintError("Error reading block");
            return -1;
        }

        /*previous block points to this block*/
        memcpy(prev_block + NEXT, &blockID, sizeof(int));
        if (BF_WriteBlock(header_info.fileDesc, prev_blockID) < 0)
            return -1;

        count = 1;
        memcpy(block + REC_NUM, &count, sizeof(int));
        memcpy(block, record, RECORD_SIZE);

        count = -1;
        memcpy(block + NEXT, &count, sizeof(int));
    }
    else {
        void *block;
        if (BF_ReadBlock(header_info.fileDesc, block_susID, &block) < 0){
            BF_PrintError("Error reading block");
            return -1;
        }
        count++;
        memcpy(block + REC_NUM, &count, sizeof(int));
        memcpy(block+(count-1)*RECORD_SIZE, record, RECORD_SIZE);

        blockID = block_susID;
    }

    if (BF_WriteBlock(header_info.fileDesc, blockID) < 0)
        return -1;
    return blockID;

}
