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
    if (memcpy(block, &info, sizeof(HP_info)))
        return 0;
    else return -1;
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

    // If blocksNum is 1, only header block is saved

    void *block_sus=NULL;
    int bfr = BLOCK_SIZE/sizeof(Record);
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
            Record *current = (block + j*sizeof(Record));
            if (record.id == current->id){
                break;
            }
        }
        if (j < count) break;
        else if (count < bfr){
            //empty_space=block + count*sizeof(Record);
            block_sus=block;
        }
    }
    if (block_sus==NULL){
        if (BF_AllocateBlock(header_info.fileDesc) < 0){
            BF_PrintError("Error allocating block");
            return -1;
        }
        void *block;
        if (BF_ReadBlock(header_info.fileDesc, 1, &block) < 0){
            BF_PrintError("Error reading block");
            return -1;
        }
        int count=1;
        memcpy(block+512-4, &count, sizeof(int));
        //block + 512 - 4
        if (memcpy(block, &record, sizeof(Record)))
            return 0;
        else return -1;
    } 
    else {
        int count;
        memcpy(count, block_sus+512-4, sizeof(int));
        memcpy(block_sus+count*sizeof(Record), &record, sizeof(Record));
    }
}
