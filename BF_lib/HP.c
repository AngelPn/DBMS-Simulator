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

    int blocksNum = BF_GetBlockCounter(header_info.fileDesc);
    void *block_sus=NULL;
    int count = 0;
    int block_num = 0;

    for (int i = 1; i < blocksNum; i++){
        void *block;
        if (BF_ReadBlock(header_info.fileDesc, i, &block) < 0){
            BF_PrintError("Error reading block");
            return -1;
        }
        memcpy(&count, block+512-4, sizeof(int));
        int j;
        for (j = 0; j < count; j++){
            Record current = (block + j*sizeof(RECORD_SIZE));

            void *record_key = get_key(record, header_info.attrName);
            void *current_key = get_key(current, header_info.attrName);
            
            if (memcmp(record_key, current_key, header_info.attrLength) == 0){
                break;
            }
        }
        if (j < count) break;
        else if (count < BLOCK_SIZE/sizeof(Record)){
            block_sus=block;
            block_num = i;
        }
    }

    if (block_sus == NULL){
        if (BF_AllocateBlock(header_info.fileDesc) < 0){
            BF_PrintError("Error allocating block");
            return -1;
        }
        void *block;
        if (BF_ReadBlock(header_info.fileDesc, 1, &block) < 0){
            BF_PrintError("Error reading block");
            return -1;
        }
        block_num = blocksNum + 1;
        count = 1;
        memcpy(block+512-4, &count, sizeof(int));
        memcpy(block, record, sizeof(RECORD_SIZE));

        //Record *cur = block;
        //printf("%s\n", cur->name);
    }
    else {
        memcpy(&count, block_sus+512-4, sizeof(int));
        memcpy(block_sus+count*sizeof(RECORD_SIZE), record, sizeof(RECORD_SIZE));

        //Record *cur = block_sus+count*sizeof(Record);
        //printf("%s\n", cur->name);
    }

    if (BF_WriteBlock(header_info.fileDesc, block_num) < 0)
        return -1;
    else return 0;
}

int HP_DeleteEntry(HP_info header_info, void *value){
    int blocksNum = BF_GetBlockCounter(header_info.fileDesc);

    for (int i = 1; i < blocksNum; i++){
        void *block;
        if (BF_ReadBlock(header_info.fileDesc, i, &block) < 0){
            BF_PrintError("Error reading block");
            return -1;
        }
        int count;
        memcpy(&count, block+512-4, sizeof(int));
        int j;
        for (j = 0; j < count; j++){

            Record current = (block + j*sizeof(RECORD_SIZE));
            void *current_key = get_key(current, header_info.attrName);

            if (memcmp(value, current_key, header_info.attrLength) == 0){

                Record last_rec = (block + count*sizeof(RECORD_SIZE));
                count--;
                memcpy(block+512-4, &count, sizeof(int));
                memcpy(last_rec, current, sizeof(RECORD_SIZE));

                if (BF_WriteBlock(header_info.fileDesc, i) < 0)
                    return -1;
                else return 0;
            }
        }
    }
    return 0;
}

int HP_GetAllEntries(HP_info header_info, void *value){
    int blocksNum = BF_GetBlockCounter(header_info.fileDesc);
    int bfr = BLOCK_SIZE/sizeof(Record);
    int i;

    for (i = 1; i < blocksNum; i++){
        void *block;
        if (BF_ReadBlock(header_info.fileDesc, i, &block) < 0){
            BF_PrintError("Error reading block");
            return -1;
        }
        int count;
        memcpy(&count, block+512-4, sizeof(int));
        int j;
        for (j = 0; j < count; j++){
            Record current = (block + j*sizeof(RECORD_SIZE));
            void *current_key = get_key(current, header_info.attrName);

            if (memcmp(value, current_key, header_info.attrLength) == 0){
                print_record(current);
                return 0;
            }
        }
    }
    return 0;
}