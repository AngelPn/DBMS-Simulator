#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "../include/HT.h"
#include "../include/BF.h"

#define NEXT_BUCKET BLOCK_SIZE-sizeof(int)
#define NEXT        BLOCK_SIZE-2*sizeof(int)
#define REC_NUM     BLOCK_SIZE-sizeof(int)

int HT_CreateIndex(char *fileName, char attrType, char* attrName,int attrLength, int buckets){
    BF_Init();
    /*Create file in block - level*/
    if (BF_CreateFile(fileName) < 0){
		BF_PrintError("Error creating file");
		return -1;
	}
    /*Open the created file in block - level and get the file identifier*/
    int fileDesc;
    if (fileDesc = BF_OpenFile(fileName) < 0){
        BF_PrintError("Error opening file");
        return -1;
    }
    /*Allocate the header block that keeps the HP_info*/
    if (BF_AllocateBlock(fileDesc) < 0){
        BF_PrintError("Error allocating block");
        return -1;
    }
    /*Read the header block and take the address*/
    void *header_block = NULL;
    if (BF_ReadBlock(fileDesc, BF_GetBlockCounter(fileDesc) - 1, &header_block) < 0){
        BF_PrintError("Error reading block");
        return -1;
    }
    HT_info info = {.fileDesc = fileDesc,
                    .attrType = attrType, 
                    .attrName = malloc(sizeof(char)*(strlen(attrName)+1)),
                    .attrLength = attrLength,
                    .numBuckets = buckets,
                    .header_block_ID = BF_GetBlockCounter(fileDesc) - 1
                    };
    strcpy(info.attrName, attrName);
    memcpy(header_block, &info, sizeof(HT_info));
    if (BF_WriteBlock(fileDesc, BF_GetBlockCounter(fileDesc) - 1) < 0)
        return -1;

    /* Calculate the number of blocks of buckets needed and the remainder*/
    int n_bucket_blocks = buckets*sizeof(int)/(BLOCK_SIZE-sizeof(int));
    int remainder = (buckets*sizeof(int))%(BLOCK_SIZE-sizeof(int));
    if (remainder > 0)
        n_bucket_blocks++;
   
    void *curr_block = NULL;
    void *prev_block = header_block;
    int bucket_blockID;
    int prev_blockID = BF_GetBlockCounter(fileDesc) - 1;
    
    for (int i = 1; i <= n_bucket_blocks; i++){
        /*Allocate a block of buckets*/
        if (BF_AllocateBlock(fileDesc) < 0){
            BF_PrintError("Error allocating block");
            exit(EXIT_FAILURE);
        }
        /*Read the block of buckets and take the address*/
        bucket_blockID = BF_GetBlockCounter(fileDesc)-1;
        if (BF_ReadBlock(fileDesc, bucket_blockID, &curr_block) < 0){
            BF_PrintError("Error reading block");
            exit(EXIT_FAILURE);
        }
        /*Point the previous block to the new block*/
        memcpy(prev_block + NEXT_BUCKET, &bucket_blockID, sizeof(int));
        if (BF_WriteBlock(fileDesc, prev_blockID) < 0)
            return -1;

        int n_buckets;
        if (i == n_bucket_blocks && remainder > 0)
            n_buckets = remainder;
        else
            n_buckets = (BLOCK_SIZE-sizeof(int))/sizeof(int);
        /*Initialize buckets with empty pointers to blocks: -1*/
        int count = -1;
        for (int j = 0; j < n_buckets; j++) {
            memcpy(curr_block + j*sizeof(int), &count, sizeof(int));
        }
        if (BF_WriteBlock(fileDesc, bucket_blockID) < 0)
            return -1;

        prev_block = curr_block;
        prev_blockID = bucket_blockID;
    }
    return 0;
}


HT_info *HT_OpenIndex(char *fileName){
    /*Get the file identifier with BF_OpenFile*/
    int fileDesc = 0;
    if (fileDesc = BF_OpenFile(fileName) < 0){
        BF_PrintError("Error opening file");
		exit(EXIT_FAILURE);
    }
    /*Read the header_block*/
    void *header_block = NULL;
    if (BF_ReadBlock(fileDesc, 0, &header_block) < 0){
        BF_PrintError("Error reading block");
        exit(EXIT_FAILURE);
    }
    /*Allocate HT_info struct*/
    HT_info *info = (HT_info *)malloc(sizeof(HT_info));

    HT_info *header_info = (HT_info *)header_block;
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
    printf("index = %d\t", index);

    int nbuckets_in_block = BLOCK_SIZE/sizeof(int) - 1; //127
    /*block ID that keeps the bucket index*/
    int blockID_bucket = index/nbuckets_in_block + 1; //+1 because of header block
    printf("blockID_bucket = %d\t", blockID_bucket);

    void *bucket_block = NULL;
    if (BF_ReadBlock(header_info.fileDesc, blockID_bucket, &bucket_block) < 0){
        BF_PrintError("Error reading block");
        return -1;
    }
    int pointer = index%nbuckets_in_block;
    //int blockID = *(int *)(bucket_block + sizeof(int)*abs((index-1) - nbuckets_in_block*(blockID_bucket-1)) -1);
    int blockID = *(int *)(bucket_block + pointer*sizeof(int));
    printf("blockID = %d\n", blockID);
 
    index = pointer*sizeof(int);
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
        index = NEXT;
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
        memcpy(prev_block + index, &blockID, sizeof(int));
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

int HT_DeleteEntry(HT_info header_info, void *value){
    printf("DELETE\n");
    int index = hash(header_info.numBuckets, value);
    printf("index = %d\t", index);

    int nbuckets_in_block = BLOCK_SIZE/sizeof(int) - 1; //127
    /*block ID that keeps the bucket index*/
    int blockID_bucket = index/nbuckets_in_block + 1; //+1 because of header block
    printf("blockID_bucket = %d\t", blockID_bucket);

    void *bucket_block = NULL;
    if (BF_ReadBlock(header_info.fileDesc, blockID_bucket, &bucket_block) < 0){
        BF_PrintError("Error reading block");
        return -1;
    }
    int pointer = index%nbuckets_in_block;
    //int blockID = *(int *)(bucket_block + sizeof(int)*abs((index-1) - nbuckets_in_block*(blockID_bucket-1)) -1);
    int blockID = *(int *)(bucket_block + pointer*sizeof(int));
    printf("blockID = %d\n", blockID);

    int count = 0;
    void *current_block;

    while ( blockID != -1) { //while (current_block != NULL)

        if (BF_ReadBlock(header_info.fileDesc, blockID, &current_block) < 0){
            BF_PrintError("Error reading block");
            return -1;
        }

        count = *(int *)(current_block + REC_NUM);

        for (int j = 0; j < count; j++){

            Record current_rec = (current_block + j*RECORD_SIZE);
            void *current_key = get_key(current_rec, header_info.attrName);

            if (memcmp(value, current_key, header_info.attrLength) == 0){

                Record last_rec = (current_block + count*RECORD_SIZE);

                count--;
                memcpy(current_block + REC_NUM, &count, sizeof(int));
                memcpy(current_block + j*RECORD_SIZE, last_rec, RECORD_SIZE);

                if (BF_WriteBlock(header_info.fileDesc, blockID) < 0)
                    return -1;
                else return 0;
            }
            blockID = *(int *)(current_block + NEXT);
        }
    }
    return -1;
}

int HT_GetAllEntries(HT_info header_info, void *value){
    printf("GET ENTRY\n");
    int index = hash(header_info.numBuckets, value);
    printf("index = %d\t", index);

    int nbuckets_in_block = BLOCK_SIZE/sizeof(int) - 1; //127
    /*block ID that keeps the bucket index*/
    int blockID_bucket = index/nbuckets_in_block + 1; //+1 because of header block
    printf("blockID_bucket = %d\t", blockID_bucket);

    void *bucket_block = NULL;
    if (BF_ReadBlock(header_info.fileDesc, blockID_bucket, &bucket_block) < 0){
        BF_PrintError("Error reading block");
        return -1;
    }
    int pointer = index%nbuckets_in_block;
    //int blockID = *(int *)(bucket_block + sizeof(int)*abs((index-1) - nbuckets_in_block*(blockID_bucket-1)) -1);
    int blockID = *(int *)(bucket_block + pointer*sizeof(int));
    printf("blockID = %d\n", blockID);

    int count = 0;
    void *current_block;

    while ( blockID != -1) { //while (current_block != NULL)

        if (BF_ReadBlock(header_info.fileDesc, blockID, &current_block) < 0){
            BF_PrintError("Error reading block");
            return -1;
        }

        count = *(int *)(current_block + REC_NUM);

        for (int j = 0; j < count; j++){

            Record current_rec = current_block + j*RECORD_SIZE;
            void *current_key = get_key(current_rec, header_info.attrName);

            if (memcmp(value, current_key, header_info.attrLength) == 0){
                print_record(current_rec);
                return blockID;
            }
        }
        blockID = *(int *)(current_block + NEXT);
    }
    return -1;
}


int HashStatistics(char *filename){
    HT_info *info  = HT_OpenIndex(filename);

     /*How many blocks has the file*/
    int nblocks = BF_GetBlockCounter(info->fileDesc);

    /*File parsing*/
    void *header_block = NULL;
    if (BF_ReadBlock(info->fileDesc, info->header_block_ID, &header_block) < 0){
        BF_PrintError("Error reading block");
        return -1;
    }

    int blockID_bucket = *(int *)(header_block + NEXT_BUCKET);
    void *bucket_block  = NULL;

    /* Calculate the number of blocks of buckets and the remainder*/
    int n_bucket_blocks = info->numBuckets*sizeof(int)/(BLOCK_SIZE-sizeof(int));
    int remainder = (info->numBuckets*sizeof(int))%(BLOCK_SIZE-sizeof(int));
    if (remainder > 0)
        n_bucket_blocks++;

    for (int i = 0; i < n_bucket_blocks; i++){
        
        if (BF_ReadBlock(info->fileDesc, blockID_bucket, &bucket_block) < 0){
            BF_PrintError("Error reading block");
            return -1;
        }

        int n_buckets;
        if (i == n_bucket_blocks && remainder > 0)
            n_buckets = remainder;
        else
            n_buckets = (BLOCK_SIZE-sizeof(int))/sizeof(int);
        /*Initialize buckets with empty pointers to blocks: -1*/
        int count = 0, pointer, blockID;
        void *current_block = NULL;

        for (int j = 0; j < n_buckets; j++) {

            pointer = j%n_bucket_blocks;
            blockID = *(int *)(bucket_block + pointer*sizeof(int));

            while ( blockID != -1) { //while (current_block != NULL)

                if (BF_ReadBlock(info->fileDesc, blockID, &current_block) < 0){
                    BF_PrintError("Error reading block");
                    return -1;
                }

                count = *(int *)(current_block + REC_NUM);

                blockID = *(int *)(current_block + NEXT);
            }
        }

        blockID_bucket = *(int *)(bucket_block + NEXT_BUCKET);
    }
}