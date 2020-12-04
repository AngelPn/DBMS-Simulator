#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "HT.h"
#include "BF.h"

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
    if (BF_ReadBlock(fileDesc, 0, &block) < 0){
        BF_PrintError("Error reading block");
        exit(EXIT_FAILURE);
    }
    HT_info info = {.fileDesc = fileDesc,
                    .attrType = attrType, 
                    .attrName = malloc(sizeof(char)*(strlen(attrName)+1)),
                    .attrLength = attrLength,
                    .numBuckets = buckets
                    };
    strcpy(info.attrName, attrName);
    memcpy(block, &info, sizeof(HT_info));
    //free(info.attrName);
    
    if (BF_WriteBlock(fileDesc, 0) < 0)
        return -1;

    int n_blocks=buckets*sizeof(int)/(BLOCK_SIZE-sizeof(int));
    int remainder=(buckets*sizeof(int))%(BLOCK_SIZE-sizeof(int));
   
    void *curr_block = NULL;
    void *prev_block = block;
    int blockID;
    for (int i=1; i<=n_blocks; i++)
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

        for (int j=0; j<(BLOCK_SIZE/sizeof(int)-1); j++) 
        {
            int count = -1;
            memcpy(curr_block+i*sizeof(int), &count, sizeof(int));
        }
        
        if (BF_WriteBlock(fileDesc,blockID) < 0)
            return -1;
        prev_block=curr_block;
    }
    if (remainder>0){
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
        for (j=0; j<remainder; j++)
        {
            int count = -1;
            memcpy(curr_block+j*sizeof(int), &count, sizeof(int));
        }
        if (BF_WriteBlock(fileDesc, 0) < 0)
            return -1;
    }
    // printf("%d\n", BF_GetBlockCounter(fileDesc));
    return 0;
}

HT_info *HT_OpenIndex(char *fileName){
    int fileDesk = 0;
    if (fileDesk = BF_OpenFile(fileName) < 0){
        BF_PrintError("Error opening file");
		exit(EXIT_FAILURE);
    }
    void *header_block = NULL;
    if (BF_ReadBlock(fileDesk, 0, &header_block) < 0){
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
