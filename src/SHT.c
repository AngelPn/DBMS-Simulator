#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "../include/SHT.h"
#include "../include/BF.h"

#define NEXT_BUCKET BLOCK_SIZE-sizeof(int)
#define NEXT        BLOCK_SIZE-2*sizeof(int)
#define REC_NUM     BLOCK_SIZE-sizeof(int)

int SHT_CreateSecondaryIndex( char *sfileName, char *fileName, char attrType, char* attrName,int attrLength, int buckets){

}

SHT_info* SHT_OpenSecondaryIndex( char *fileName){

}

int SHT_CloseSecondaryIndex( SHT_info* header_info){

}

int SHT_SecondaryInsertEntry( SHT_info header_info, SecondaryRecord record){

}

int SHT_SecondaryGetAllEntries(SHT_info header_info_sht, HT_info header_info_ht, void *value){

}

int HashStatistics(char *filename){
    
}