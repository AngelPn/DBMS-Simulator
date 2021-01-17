#include <stdio.h>
#include <stdlib.h>
#include "../include/HashStatistics.h"
#include "../include/BF.h"
#include "SHT.h"
#include "HT.h"

#define NEXT_BUCKET BLOCK_SIZE-sizeof(int)
#define NEXT        BLOCK_SIZE-2*sizeof(int)
#define REC_NUM     BLOCK_SIZE-sizeof(int)

int HashStatistics(char *filename){

    /*Get the file identifier with BF_OpenFile*/
    int fileDesc = 0;
    if ((fileDesc = BF_OpenFile(filename)) < 0){
        BF_PrintError("Error opening file");
		return -1;
    }
    /*Read the header_block*/
    void *header_block = NULL;
    if (BF_ReadBlock(fileDesc, 0, &header_block) < 0){
        BF_PrintError("Error reading block");
		return -1;
    }

    int ftype = *(int *)(header_block);
    int numBuckets;
    if (ftype == 1) {
        HT_info *header_info = (HT_info *)header_block;
        numBuckets = header_info->numBuckets;
    }
    else if (ftype == 2){
        SHT_info *header_info = (SHT_info *)header_block;
        numBuckets = header_info->numBuckets;
    }
    /*How many blocks has the file*/
    int nblocks = BF_GetBlockCounter(fileDesc);

    int min_recs = 0, max_recs = -1;
    int total_recs = 0, total_blocks_bucket = 0;
    int overflow_buckets = 0, overflow_blocks;

    /*File parsing*/

    int blockID_bucket = *(int *)(header_block + NEXT_BUCKET);
    void *bucket_block  = NULL;

    /* Calculate the number of blocks of buckets and the remainder*/
    int n_bucket_blocks = numBuckets*sizeof(int)/(BLOCK_SIZE-sizeof(int));
    int remainder = (numBuckets*sizeof(int))%(BLOCK_SIZE-sizeof(int));
    if (remainder > 0)
        n_bucket_blocks++;

    int bucket_index = 0;
    for (int i = 1; i <= n_bucket_blocks; i++){ /*For every block of buckets*/
        
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
            
            if (BF_ReadBlock(fileDesc, blockID_bucket, &bucket_block) < 0){
                BF_PrintError("Error reading block");
                return -1;
            }

            int count = 0;
            blockID = *(int *)(bucket_block + j*sizeof(int)); /*ID of bucket's block of records*/

            while (blockID != -1) { /*for every block of records in bucket*/

                total_blocks_bucket++;
                overflow_blocks++;
                if (BF_ReadBlock(fileDesc, blockID, &current_block) < 0){
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
        // get the address of blockID_bucket
        if (BF_ReadBlock(fileDesc, blockID_bucket, &bucket_block) < 0){ /**/
            BF_PrintError("Error reading block");
            return -1;
        }
        blockID_bucket = *(int *)(bucket_block + NEXT_BUCKET);
    }
    printf("Number of blocks in file %d.\n", nblocks);

    float average_recs = (float)total_recs / (float)numBuckets; /*b*/
    printf("Minimum number of records in bucket is %d.\n", min_recs);
    printf("Maximum number of records in bucket is %d.\n", max_recs);
    printf("Average number of records in bucket is %f.\n", average_recs);

    float average_blocks_bucket = (float)total_blocks_bucket/ (float)numBuckets; /*c*/
    printf("Average number of blocks in a bucket %f.\n", average_blocks_bucket);

    printf("%d buckets have overflow blocks.\n", overflow_buckets);
    
    return 0;
}
