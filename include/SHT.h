#ifndef SHT_H
#define SHT_H

#include "Record.h"
#include "HT.h"

#define SEC_RECORD_SIZE 98

typedef struct SHT_info
{
    int fileType; //type of the file: 0->heap, 1->hash, 2->secondary hash
    int fileDesc;
    char *fileName; //filename of hash table
    char *attrName; //name of key
    int attrLength; //size of key
    long int numBuckets;
    int header_block_ID; //block ID of header block
} SHT_info;

typedef struct
{
    Record record;
    int blockId;
} SecondaryRecord;

typedef struct
{
    char surname[25];
    int blockId;
} SHTrecord;


int SHT_CreateSecondaryIndex( char *sfileName, char* attrName, int attrLength, int buckets, char *fileName);

SHT_info* SHT_OpenSecondaryIndex( char *sfileName); 

int SHT_CloseSecondaryIndex( SHT_info* header_info);

int SHT_SecondaryInsertEntry( SHT_info header_info, SecondaryRecord record);

int SHT_SecondaryGetAllEntries(SHT_info header_info_sht, HT_info header_info_ht, void *value);

#endif /*SHT_H*/