#ifndef HT_H
#define HT_H

#include "Record.h"

typedef struct HT_info
{
    int fileType; //type of the file: 0->heap, 1->hash
    int fileDesc;
    char attrType; //type of key
    char *attrName; //name of key
    int attrLength; //size of key
    long int numBuckets;
    int header_block_ID; //block ID of header block
} HT_info;

int HT_CreateIndex( char *fileName, char attrType, char* attrName,int attrLength, int buckets);

HT_info* HT_OpenIndex( char *fileName); 

int HT_CloseIndex( HT_info* header_info);

int HT_InsertEntry( HT_info header_info,Record record);

int HT_DeleteEntry(HT_info header_info, void *value);

int HT_GetAllEntries(HT_info header_info, void *value);

#endif /*HT_H*/