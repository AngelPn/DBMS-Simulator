#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "HP.h"
#include "BF.h"

int HP_CreateFile(char *fileName, char attrType, char *attrName, int attrLength){
    BF_Init();
    if (BF_CreateFile(fileName) < 0) {
		BF_PrintError("Error creating file");
		exit(EXIT_FAILURE);
	}
    int fileDesc;
    if (fileDesc = BF_OpenFile(fileName) < 0){
        BF_PrintError("Error opening file");
        exit(EXIT_FAILURE);
    }
    if (BF_AllocateBlock(fileDesc) < 0){
        BF_PrintError("Error allocating block");
        exit(EXIT_FAILURE);
    }
    void *block;
    if (BF_ReadBlock(fileDesc, 0, &block) < 0){
        BF_PrintError("Error reading block");
        exit(EXIT_FAILURE);
    }
    HP_info info = {.fileDesc = fileDesc,
                    .attrType = attrType, 
                    .attrName = malloc(strlen(attrName)+1),
                    .attrLength = attrLength
                    };
    strcpy(info.attrName, attrName);
    memcpy(block, &info, sizeof(HP_info));
    //memcpy(block+512-8, NULL, 4);
    if (BF_WriteBlock(fileDesc, 0) < 0)
        return -1;
    else return 0;
}

HP_info *HP_OpenFile(char *fileName){
    int fileDesk = 0;
    if (fileDesk = BF_OpenFile(fileName) < 0){
        BF_PrintError("Error opening file");
		exit(EXIT_FAILURE);
    }
    void *header_block;
    if (BF_ReadBlock(fileDesk, 0, &header_block) < 0){
        BF_PrintError("Error reading block");
        exit(EXIT_FAILURE);
    }
    HP_info *info = malloc(sizeof(HP_info));
    info->attrName = malloc(strlen(header_block + 6) + 1);
    memcpy(info, header_block, sizeof(HP_info));
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

    void *block_sus=NULL;
    int count = 0;
    int block_num = 0;
    void *current_block, *previous_block;
    if (BF_ReadBlock(header_info.fileDesc, 0,  &current_block) < 0){
        BF_PrintError("Error reading block");
        return -1;
    }
    previous_block = current_block;
    current_block = current_block+512-8;

    while ( current_block != NULL) {
        block_num++;
        memcpy(&count, current_block+512-4, sizeof(int));
        int j;
        for (j = 0; j < count; j++){
            Record current_rec = (current_block + j*sizeof(RECORD_SIZE));

            void *record_key = get_key(record, header_info.attrName);
            void *current_key = get_key(current_rec, header_info.attrName);
            
            if (memcmp(record_key, current_key, header_info.attrLength) == 0){
                return -1;
            }
        }
        if (count < BLOCK_SIZE/sizeof(Record)){
            block_sus=current_block;
        }
        previous_block = current_block;
        current_block = current_block+512-8;
    }

    if (block_sus == NULL){ /*if all previous blocks are full*/
        if (BF_AllocateBlock(header_info.fileDesc) < 0){
            BF_PrintError("Error allocating block");
            return -1;
        }
        void *block;
        if (BF_ReadBlock(header_info.fileDesc, BF_GetBlockCounter(header_info.fileDesc), &block) < 0){
            BF_PrintError("Error reading block");
            return -1;
        }
        memcpy(previous_block+512-8, &block, 4); /*previous block points to this block*/
        count = 1;
        memcpy(block+512-4, &count, sizeof(int));
        memcpy(block, record, sizeof(RECORD_SIZE));

        //memcpy(block+512-8, NULL, 4); /*this block points to null-it is the last block*/

        //Record *cur = block;
        //printf("%s\n", cur->name);
        block_num++;
    }
    else {
        memcpy(&count, block_sus+512-4, sizeof(int));
        memcpy(block_sus+count*sizeof(RECORD_SIZE), record, sizeof(RECORD_SIZE));

        //Record *cur = block_sus+count*sizeof(Record);
        //printf("%s\n", cur->name);
    }

    if (BF_WriteBlock(header_info.fileDesc, block_num) < 0)
        return -1;
    return block_num;
}

int HP_DeleteEntry(HP_info header_info, void *value){
    void *current_block; 
    int block_num=0;
    if (BF_ReadBlock(header_info.fileDesc, 0,  &current_block) < 0){
        BF_PrintError("Error reading block");
        return -1;
    }
    current_block = current_block+512-8;

    while (current_block != NULL) {
        void *block;
        block_num++;

        int count;
        memcpy(&count, block+512-4, sizeof(int));
   
        for (int j = 0; j < count; j++){

            Record current_rec = (block + j*sizeof(RECORD_SIZE));
            void *current_key = get_key(current_rec, header_info.attrName);

            if (memcmp(value, current_key, header_info.attrLength) == 0){

                Record last_rec = (block + count*sizeof(RECORD_SIZE));
                count--;
                memcpy(block+512-4, &count, sizeof(int));
                memcpy(last_rec, current_rec, sizeof(RECORD_SIZE));

                if (BF_WriteBlock(header_info.fileDesc, block_num) < 0)
                    return -1;
                else return 0;
            }
        }
        current_block = current_block+512-8;
    }
    return -1;
}

int HP_GetAllEntries(HP_info header_info, void *value){
    
    void *current_block; 
    int block_num=0;
    if (BF_ReadBlock(header_info.fileDesc, 0,  &current_block) < 0){
        BF_PrintError("Error reading block");
        return -1;
    }
    current_block = current_block+512-8;

    while (current_block != NULL) {
        void *block;
        block_num++;
    
        int count;
        memcpy(&count, block+512-4, sizeof(int));
    
        for (int j = 0; j < count; j++){
            Record current_rec = (block + j*sizeof(RECORD_SIZE));
            void *current_key = get_key(current_rec, header_info.attrName);

            if (memcmp(value, current_key, header_info.attrLength) == 0){
                print_record(current_rec);
                return block_num;
            }
        }
    }
    return -1;
}