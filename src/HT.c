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
    if ((fileDesc = BF_OpenFile(fileName)) < 0){
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
    int remainder= (buckets*sizeof(int))%(BLOCK_SIZE-sizeof(int));
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
		    return -1;
        }
        /*Read the block of buckets and take the address*/
        bucket_blockID = BF_GetBlockCounter(fileDesc)-1;
        if (BF_ReadBlock(fileDesc, bucket_blockID, &curr_block) < 0){
            BF_PrintError("Error reading block");
		    return -1;
        }
        /*Point the previous block to the new block*/
        memcpy(prev_block + NEXT_BUCKET, &bucket_blockID, sizeof(int));
        if (BF_WriteBlock(fileDesc, prev_blockID) < 0)
            return -1;

        int n_buckets;
        if (i == n_bucket_blocks && remainder > 0)
            n_buckets = remainder/sizeof(int);
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
    if ((fileDesc = BF_OpenFile(fileName)) < 0){
        BF_PrintError("Error opening file");
		return NULL;
    }
    /*Read the header_block*/
    void *header_block = NULL;
    if (BF_ReadBlock(fileDesc, 0, &header_block) < 0){
        BF_PrintError("Error reading block");
		return NULL;
    }
    /*Allocate HT_info struct*/
    HT_info *info = (HT_info *)malloc(sizeof(HT_info));

    HT_info *header_info = (HT_info *)header_block;
    memcpy(info, header_info, sizeof(HT_info));

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
    return k%nbuckets;
}


int HT_InsertEntry(HT_info header_info, Record record){

    void *key = get_key(record, header_info.attrName);
    int index = hash(header_info.numBuckets, key);

    /*File parsing*/
    void *header_block = NULL;
    if (BF_ReadBlock(header_info.fileDesc, header_info.header_block_ID, &header_block) < 0){
        BF_PrintError("Error reading block");
        return -1;
    }

    int blockID_bucket = *(int *)(header_block + NEXT_BUCKET);
    void *bucket_block  = NULL;
    int n_buckets = (BLOCK_SIZE-sizeof(int))/sizeof(int);
    int bucket_index = -1;
    int i;

    while (blockID_bucket != -1){ /*For every block of buckets*/

        /*Read the block of buckets with blockID_bucket and get the address*/
        if (BF_ReadBlock(header_info.fileDesc, blockID_bucket, &bucket_block) < 0){
            BF_PrintError("Error reading block");
            return -1;
        }
        
        for (i = 0; i < n_buckets ; i++){ /*For every bucket in block of buckets*/
            bucket_index++;
            if (index == bucket_index) break;
        }
        if (index == bucket_index){
            index = i*sizeof(int);
            break;
        }
        blockID_bucket = *(int *)(bucket_block + NEXT_BUCKET);
    }

    int count = 0;
    int block_susID = -1, prev_blockID = blockID_bucket;

    int blockID = *(int *)(bucket_block + index); /*ID of block of records*/

    void *current_block = NULL;

    while (blockID != -1) {  /*Scan the blocks of records: do not insert a record that already exists*/

        /*Read the block with ID blockID and get the address*/
        if (BF_ReadBlock(header_info.fileDesc, blockID, &current_block) < 0){
            BF_PrintError("Error reading block");
            return -1;
        }
        /*Get the number of records of current_block and store it to count variable*/
        count = *(int *)(current_block + REC_NUM);

        for (int j = 0; j < count; j++){ /*For every record in current_block, compare keys*/
            Record *current_rec = (Record *)(current_block + j*RECORD_SIZE);
            void *record_key = get_key(record, header_info.attrName);
            void *current_key = get_key(*current_rec, header_info.attrName);
            if (memcmp(record_key, current_key, header_info.attrLength) == 0){
                return -1;
            }
        }
        /*If there is empty space in block for record, store the ID of block in block_susID*/
        if (count < BLOCK_SIZE/RECORD_SIZE){
            block_susID = blockID;
        }
        prev_blockID = blockID;
        blockID = *(int *)(current_block + NEXT);
        index = NEXT;
    }

    if (block_susID == -1){ /*If all previous blocks are full*/
        if (BF_AllocateBlock(header_info.fileDesc) < 0){
            BF_PrintError("Error allocating block");
            return -1;
        }
        /*Get the ID of new allocated block and store it in blockID*/
        blockID = BF_GetBlockCounter(header_info.fileDesc)-1;
        /*Read the block with ID blockID and get the address*/
        void *block;
        if (BF_ReadBlock(header_info.fileDesc, blockID, &block) < 0){
            BF_PrintError("Error reading block");
            return -1;
        }
        /*Read the previous block and get the address*/
        void *prev_block;
        if (BF_ReadBlock(header_info.fileDesc, prev_blockID, &prev_block) < 0){
            BF_PrintError("Error reading block");
            return -1;
        }
        /*Make previous block point to new allocated block*/
        /*If previous block is block of buckets, index is the bucket index*/
        /*If previous block is block of records, index is NEXT*/
        memcpy(prev_block + index, &blockID, sizeof(int));
        if (BF_WriteBlock(header_info.fileDesc, prev_blockID) < 0)
            return -1;
        /*Add the record in new allocated block, set number of records to 1 and pointer to next block -1*/
        memcpy(block, &record, RECORD_SIZE);
        count = 1;
        memcpy(block + REC_NUM, &count, sizeof(int));
        count = -1;
        memcpy(block + NEXT, &count, sizeof(int));
    }
    else { /*If an empty space found, add the record there*/
        void *block;
        if (BF_ReadBlock(header_info.fileDesc, block_susID, &block) < 0){
            BF_PrintError("Error reading block");
            return -1;
        }
        count++;
        memcpy(block + REC_NUM, &count, sizeof(int));
        memcpy(block+(count-1)*RECORD_SIZE, &record, RECORD_SIZE);

        blockID = block_susID;
    }
    /*Return the ID of block that record has inserted*/
    if (BF_WriteBlock(header_info.fileDesc, blockID) < 0)
        return -1;
    return blockID;
}


int HT_DeleteEntry(HT_info header_info, void *value){

    int index = hash(header_info.numBuckets, value);

    /*File parsing*/
    void *header_block = NULL;
    if (BF_ReadBlock(header_info.fileDesc, header_info.header_block_ID, &header_block) < 0){
        BF_PrintError("Error reading block");
        return -1;
    }

    int blockID_bucket = *(int *)(header_block + NEXT_BUCKET);
    void *bucket_block  = NULL;
    int n_buckets = (BLOCK_SIZE-sizeof(int))/sizeof(int);
    int bucket_index = -1;
    int i;

    while (blockID_bucket != -1){ /*For every block of buckets*/

        /*Read the block of buckets with blockID_bucket and get the address*/
        if (BF_ReadBlock(header_info.fileDesc, blockID_bucket, &bucket_block) < 0){
            BF_PrintError("Error reading block");
            return -1;
        }
        
        for (i = 0; i < n_buckets ; i++){ /*For every bucket in block of buckets*/
            bucket_index++;
            if (index == bucket_index) break;
        }
        if (index == bucket_index){
            index = i*sizeof(int);
            break;
        }
        blockID_bucket = *(int *)(bucket_block + NEXT_BUCKET);
    }

    int blockID = *(int *)(bucket_block + index); /*ID of block of records*/
    int count = 0;
    void *current_block = NULL;

    while (blockID != -1) { /*Scan the blocks of records*/

        /*Read the block with ID blockID and get the address*/
        if (BF_ReadBlock(header_info.fileDesc, blockID, &current_block) < 0){
            BF_PrintError("Error reading block");
            return -1;
        }
        /*Get the number of records of current_block and store it to count variable*/
        count = *(int *)(current_block + REC_NUM);

        for (int j = 0; j < count; j++){ /*For every record in current_block, compare keys*/

            Record *current_rec = (current_block + j*RECORD_SIZE);
            void *current_key = get_key(*current_rec, header_info.attrName);

            if (memcmp(value, current_key, header_info.attrLength) == 0){ /*Record to delete is found*/
                /*Get the last record of current block and copy it to the record to delete*/
                Record *last_rec = (current_block + count*RECORD_SIZE);
                /*Decreament the number of records*/
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

    int index = hash(header_info.numBuckets, value);

    /*File parsing*/
    void *header_block = NULL;
    if (BF_ReadBlock(header_info.fileDesc, header_info.header_block_ID, &header_block) < 0){
        BF_PrintError("Error reading block");
        return -1;
    }
    
    int blockID_bucket = *(int *)(header_block + NEXT_BUCKET);
    void *bucket_block  = NULL;
    int n_buckets = (BLOCK_SIZE-sizeof(int))/sizeof(int);
    int bucket_index = -1;
    int i;
    int block_counter = 0;

    while (blockID_bucket != -1){ /*For every block of buckets*/

        /*Read the block of buckets with blockID_bucket and get the address*/
        if (BF_ReadBlock(header_info.fileDesc, blockID_bucket, &bucket_block) < 0){
            BF_PrintError("Error reading block");
            return -1;
        }
        
        for (i = 0; i < n_buckets ; i++){ /*For every bucket in block of buckets*/
            bucket_index++;
            if (index == bucket_index) break;
        }
        if (index == bucket_index){
            index = i*sizeof(int);
            break;
        }
        blockID_bucket = *(int *)(bucket_block + NEXT_BUCKET);
    }

    int blockID = *(int *)(bucket_block + index); /*ID of block of records*/

    int count = 0;
    void *current_block = NULL;

    while (blockID != -1) { /*Scan the blocks of records*/
        block_counter++;

        /*Read the block with ID blockID and get the address*/
        if (BF_ReadBlock(header_info.fileDesc, blockID, &current_block) < 0){
            BF_PrintError("Error reading block");
            return -1;
        }
        /*Get the number of records of current_block and store it to count variable*/
        count = *(int *)(current_block + REC_NUM);

        for (int j = 0; j < count; j++){ /*For every record in current_block, compare keys*/

            Record *current_rec = current_block + j*RECORD_SIZE;
            void *current_key = get_key(*current_rec, header_info.attrName);
            if (value == NULL){  /*if value is null print every entry*/
                print_record(*current_rec);
            } 
            else if (memcmp(value, current_key, header_info.attrLength) == 0){ /*Record to print is found*/
                print_record(*current_rec);
                return block_counter;
            }
        }
        blockID = *(int *)(current_block + NEXT);
    }
    if (value == NULL) {
        return block_counter;
    }
    return -1;
}


int HashStatistics(char *filename){

    HT_info *info  = HT_OpenIndex(filename);

     /*How many blocks has the file*/
    int nblocks = BF_GetBlockCounter(info->fileDesc);

    int min_recs = 0, max_recs = -1;
    int total_recs = 0, total_blocks_bucket = 0;
    int overflow_buckets = 0, overflow_blocks;

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

    int bucket_index = 0;
    for (int i = 1; i <= n_bucket_blocks; i++){ /*For every block of buckets*/
        
        if (BF_ReadBlock(info->fileDesc, blockID_bucket, &bucket_block) < 0){
            BF_PrintError("Error reading block");
            return -1;
        }

        int n_buckets;
        if (i == n_bucket_blocks && remainder > 0)
            n_buckets = remainder/sizeof(int);
        else
            n_buckets = (BLOCK_SIZE-sizeof(int))/sizeof(int);

        int blockID;
        void *current_block = NULL;
        for (int j = 0; j < n_buckets; j++) { /*For every bucket in block of buckets*/
            
            bucket_index++;
            overflow_blocks = 0;
            int count = 0;
            blockID = *(int *)(bucket_block + (j%n_bucket_blocks)*sizeof(int));

            while (blockID != -1) { /*for every block of records in bucket*/

                total_blocks_bucket++;
                overflow_blocks++;
                if (BF_ReadBlock(info->fileDesc, blockID, &current_block) < 0){
                    BF_PrintError("Error reading block");
                    return -1;
                }
                count += *(int *)(current_block + REC_NUM);

                blockID = *(int *)(current_block + NEXT);
            }
            if (i == 1 && j == 0) /*Initialize min_recs*/
                min_recs = count;
            if (count < min_recs)
                min_recs = count;
            if (count > max_recs)
                max_recs = count;
            total_recs += count;

            if (overflow_blocks > 1){
                overflow_buckets++;
                printf("Bucket %d has %d overflow blocks.\n", bucket_index, overflow_blocks-1);
            }
        }
        blockID_bucket = *(int *)(bucket_block + NEXT_BUCKET);
    }
    printf("Number of blocks in file %d.\n", nblocks);

    float average_recs = (float)total_recs / (float)info->numBuckets; /*b*/
    printf("Minimum number of records in bucket is %d.\n", min_recs);
    printf("Maximum number of records in bucket is %d.\n", max_recs);
    printf("Average number of records in bucket is %f.\n", average_recs);

    float average_blocks_bucket = (float)total_blocks_bucket/ (float)info->numBuckets; /*c*/
    printf("Average number of blocks in a bucket %f.\n", average_blocks_bucket);

    printf("%d buckets have overflow blocks.\n", overflow_buckets);
    
    free(info);

    return 0;
}