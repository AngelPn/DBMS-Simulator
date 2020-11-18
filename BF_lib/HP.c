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
    int fileDesk;
    if (fileDesk = BF_OpenFile(fileName) < 0){
        BF_PrintError("Error opening file");
        exit(EXIT_FAILURE);
    }
    if (BF_AllocateBlock(fileDesk) < 0){
        BF_PrintError("Error allocating block");
        exit(EXIT_FAILURE);
    }
    void *block;
    printf("%d\n", BF_GetBlockCounter(fileDesk));
    if (BF_ReadBlock(fileDesk, 0, &block) < 0){
        BF_PrintError("Error reading block");
        exit(EXIT_FAILURE);
    }
    HP_info info = {.fileDesc = fileDesk,
                    .attrType = attrType, 
                    .attrName = attrName,
                    .attrLength = attrLength
                    };
    if (memcpy(block, &info, sizeof(HP_info)))
        return 0;
    else return -1;
}