#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/HP.h"
#include "../include/BF.h"

#define NEXT    BLOCK_SIZE-2*sizeof(int)
#define REC_NUM BLOCK_SIZE-sizeof(int)

int HP_CreateFile(char *fileName, char attrType, char *attrName, int attrLength){
    BF_Init();
    /*Create file in block - level*/
    if (BF_CreateFile(fileName) < 0) {
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
    /*Create the HP_info struct and save it to header_block*/
    HP_info info = {.fileDesc = fileDesc,
                    .attrType = attrType, 
                    .attrName = malloc(sizeof(char)*(strlen(attrName)+1)),
                    .attrLength = attrLength,
                    .header_block_ID = BF_GetBlockCounter(fileDesc) - 1
                    };
    strcpy(info.attrName, attrName);
    memcpy(header_block, &info, sizeof(HP_info));
    /*Represent that next block is empty with -1*/
    int count = -1;
    memcpy(header_block + NEXT, &count, sizeof(int));
    
    if (BF_WriteBlock(fileDesc, 0) < 0)
        return -1;
    else return 0;
}


HP_info *HP_OpenFile(char *fileName){
    /*Get the file identifier with BF_OpenFile*/
    int fileDesk = 0;
    if (fileDesk = BF_OpenFile(fileName) < 0){
        BF_PrintError("Error opening file");
		return NULL;
    }
    /*Read the header_block*/
    void *header_block = NULL;
    if (BF_ReadBlock(fileDesk, 0, &header_block) < 0){
        BF_PrintError("Error reading block");
        return NULL;
    }
    /*Allocate HP_info struct*/
    HP_info *info = (HP_info *)malloc(sizeof(HP_info));
    
    HP_info *header_info = (HP_info *)header_block;
    memcpy(info, header_info, sizeof(HP_info));

    return info;
}


int HP_CloseFile(HP_info *header_info){
    if (BF_CloseFile(header_info->fileDesc) < 0){
        BF_PrintError("Error closing file");
        return -1;
    }
    free(header_info->attrName);
    free(header_info);
    return 0;
}

int HP_InsertEntry(HP_info header_info, Record record){

    int blockID = 0, prev_blockID = 0, block_susID = -1;
    int count = 0;

    /*Get the block ID of first block of records*/
    void *current_block = NULL;
    if (BF_ReadBlock(header_info.fileDesc, header_info.header_block_ID, &current_block) < 0){
        BF_PrintError("Error reading block");
        return -1;
    }
    blockID = *(int *)(current_block + NEXT);

    /*Scan the blocks of records: do not insert a record that already exists*/
    while (blockID != -1) { 
        /*Read the block with ID blockID and get the address*/
        if (BF_ReadBlock(header_info.fileDesc, blockID, &current_block) < 0){
            BF_PrintError("Error reading block");
            return -1;
        }
        /*Get the number of records of current_block and store it to count variable*/
        count = *(int *)(current_block + REC_NUM);

        for (int j = 0; j < count; j++){/*For every record in current_block, compare keys*/
            Record current_rec = (Record)(current_block + j*RECORD_SIZE);
            void *record_key = get_key(record, header_info.attrName);
            void *current_key = get_key(current_rec, header_info.attrName);
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
    }

    if (block_susID == -1){ /*If all previous blocks are full*/
        if (BF_AllocateBlock(header_info.fileDesc) < 0){
            BF_PrintError("Error allocating block");
            return -1;
        }
        /*Get the ID of new allocated block and store it in blockID*/
        blockID = BF_GetBlockCounter(header_info.fileDesc)-1;
        /*Read the block with ID blockID and get the address*/
        void *block = NULL;
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
        memcpy(prev_block + NEXT, &blockID, sizeof(int));
        if (BF_WriteBlock(header_info.fileDesc, prev_blockID) < 0)
            return -1;
        /*Add the record in new allocated block, set number of records to 1 and pointer to next block -1*/
        memcpy(block, record, RECORD_SIZE);
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
        memcpy(block+(count-1)*RECORD_SIZE, record, RECORD_SIZE);

        blockID = block_susID;
    }
    /*Return the ID of block that record has inserted*/
    if (BF_WriteBlock(header_info.fileDesc, blockID) < 0)
        return -1;
    return blockID;
}


int HP_DeleteEntry(HP_info header_info, void *value){

    int count = 0;
    int blockID = 0;

    /*Get the block ID of first block of records*/
    void *current_block = NULL;
    if (BF_ReadBlock(header_info.fileDesc, header_info.header_block_ID, &current_block) < 0){
        BF_PrintError("Error reading block");
        return -1;
    }
    blockID = *(int *)(current_block + NEXT);

    while (blockID != -1) { /*Scan the blocks of records*/
        /*Read the block with ID blockID and get the address*/
        if (BF_ReadBlock(header_info.fileDesc, blockID, &current_block) < 0){
            BF_PrintError("Error reading block");
            return -1;
        }
        /*Get the number of records of current_block and store it to count variable*/
        count = *(int *)(current_block + REC_NUM);

        for (int j = 0; j < count; j++){ /*For every record in current_block, compare keys*/

            Record current_rec = (current_block + j*RECORD_SIZE);
            void *current_key = get_key(current_rec, header_info.attrName);

            if (memcmp(value, current_key, header_info.attrLength) == 0){/*Record to delete is found*/
                /*Get the last record of current block and copy it to the record to delete*/
                Record last_rec = (current_block + count*RECORD_SIZE);
                memcpy(current_block + j*RECORD_SIZE, last_rec, RECORD_SIZE);
                /*Decreament the number of records*/
                count--;
                memcpy(current_block + REC_NUM, &count, sizeof(int));

                if (BF_WriteBlock(header_info.fileDesc, blockID) < 0)
                    return -1;
                else return 0;
            }
            blockID = *(int *)(current_block + NEXT);
        }
    }
    return -1;
}


int HP_GetAllEntries(HP_info header_info, void *value){

    int count = 0;
    int blockID = 0;

    /*Get the block ID of first block of records*/
    void *current_block = NULL;
    if (BF_ReadBlock(header_info.fileDesc, header_info.header_block_ID, &current_block) < 0){
        BF_PrintError("Error reading block");
        return -1;
    }
    blockID = *(int *)(current_block + NEXT);

    while (blockID != -1) {/*Scan the blocks of records*/
        /*Read the block with ID blockID and get the address*/
        if (BF_ReadBlock(header_info.fileDesc, blockID, &current_block) < 0){
            BF_PrintError("Error reading block");
            return -1;
        }
        /*Get the number of records of current_block and store it to count variable*/
        count = *(int *)(current_block + REC_NUM);

        for (int j = 0; j < count; j++){ /*For every record in current_block, compare keys*/

            Record current_rec = current_block + j*RECORD_SIZE;
            void *current_key = get_key(current_rec, header_info.attrName);

            if (memcmp(value, current_key, header_info.attrLength) == 0){ /*Record to print is found*/
                print_record(current_rec);
                return blockID;
            }
        }
        blockID = *(int *)(current_block + NEXT);
    }
    return -1;
}