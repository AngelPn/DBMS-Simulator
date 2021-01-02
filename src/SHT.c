#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "../include/SHT.h"
#include "../include/BF.h"

#define NEXT_BUCKET BLOCK_SIZE-sizeof(int)
#define NEXT        BLOCK_SIZE-2*sizeof(int)
#define SEC_REC_NUM     BLOCK_SIZE-sizeof(int)

int SHT_CreateSecondaryIndex( char *sfileName, char* attrName,int attrLength, int buckets, char *fileName){
    /*Create file in block - level*/
    if (BF_CreateFile(sfileName) < 0){
		BF_PrintError("Error creating file");
		return -1;
	}
    /*Open the created file in block - level and get the file identifier*/
    int fileDesc;
    if ((fileDesc = BF_OpenFile(sfileName)) < 0){
        BF_PrintError("Error opening file");
        return -1;
    }
    /*Allocate the header block that keeps the SHT_info*/
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
    SHT_info info = {.fileType = 2,
                    .fileDesc = fileDesc,
                    .fileName = malloc(sizeof(char)*(strlen(fileName)+1)),
                    .attrName = malloc(sizeof(char)*(strlen(attrName)+1)),
                    .attrLength = attrLength,
                    .numBuckets = buckets,
                    .header_block_ID = BF_GetBlockCounter(fileDesc) - 1
                    };
    strcpy(info.fileName, fileName);
    strcpy(info.attrName, attrName);
    memcpy(header_block, &info, sizeof(SHT_info));
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

SHT_info* SHT_OpenSecondaryIndex( char *sfileName){
    /*Get the file identifier with BF_OpenFile*/
    int fileDesc = 0;
    if ((fileDesc = BF_OpenFile(sfileName)) < 0){
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
    SHT_info *info = (SHT_info *)malloc(sizeof(SHT_info));

    SHT_info *header_info = (SHT_info *)header_block;
    /*Check if the file type is secondary hash*/
    if (header_info->fileType != 2)
        return NULL;
    memcpy(info, header_info, sizeof(SHT_info));

    return info;
}

int SHT_CloseSecondaryIndex( SHT_info* header_info){
    if (BF_CloseFile(header_info->fileDesc) < 0){
        BF_PrintError("Error closing file");
        return -1;
    }
    free(header_info->attrName);
    free(header_info->fileName);
    free(header_info);
    return 0;
}

int sht_hash(long int nbuckets, void *key){
    char *k = (char *)key;

    int hashing = 0;
    for (int i=0; i< strlen(key); i++){
        hashing += k[i];
    }
    return hashing%nbuckets;
}

int SHT_SecondaryInsertEntry( SHT_info header_info, SecondaryRecord record){

    void *key = get_key(&record.record, header_info.attrName);
    int index = sht_hash(header_info.numBuckets, key);

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

    while (blockID != -1) {  /*Scan the blocks of SecondaryRecords: do not insert a record that already exists*/

        /*Read the block with ID blockID and get the address*/
        if (BF_ReadBlock(header_info.fileDesc, blockID, &current_block) < 0){
            BF_PrintError("Error reading block");
            return -1;
        }
        /*Get the number of SecondaryRecords of current_block and store it to count variable*/
        count = *(int *)(current_block + SEC_REC_NUM);

        for (int j = 0; j < count; j++){ /*For every SecondaryRecord in current_block, compare keys*/
            SecondaryRecord *current_rec = (SecondaryRecord *)(current_block + j*(sizeof(SecondaryRecord)));
            void *record_key = get_key(&record.record, header_info.attrName); //surname
            void *current_key = get_key(&(current_rec->record), header_info.attrName);
            if (memcmp(record_key, current_key, header_info.attrLength) == 0 && record.blockId == current_rec->blockId){
                return 0;
            }
        }
        /*If there is empty space in block for SecondaryRecord, store the ID of block in block_susID*/
        if (count < BLOCK_SIZE/sizeof(SecondaryRecord)){
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
        /*If previous block is block of SecondaryRecords, index is NEXT*/
        memcpy(prev_block + index, &blockID, sizeof(int));
        if (BF_WriteBlock(header_info.fileDesc, prev_blockID) < 0)
            return -1;
        /*Add the record in new allocated block, set number of records to 1 and pointer to next block -1*/
        memcpy(block, &record, sizeof(SecondaryRecord));
        count = 1;
        memcpy(block + SEC_REC_NUM, &count, sizeof(int));
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
        memcpy(block + SEC_REC_NUM, &count, sizeof(int));
        memcpy(block+(count-1)*RECORD_SIZE, &record, RECORD_SIZE);
    }
    /*Return the ID of block that record has inserted*/
    if (BF_WriteBlock(header_info.fileDesc, blockID) < 0)
        return -1;
    return 0;
}

int SHT_SecondaryGetAllEntries(SHT_info header_info_sht, HT_info header_info_ht, void *value){

}
