#ifndef HP_H
#define HP_H

#include "Record.h"

typedef struct HP_info
{
    int fileDesc;
    char attrType; //type of key
    char *attrName; //name of key
    int attrLength; //size of key
    int header_block_ID; //block ID of header block
} HP_info;


int HP_CreateFile(char *fileName, char attrType, char *attrName, int attrLength);

HP_info *HP_OpenFile(char *fileName);

int HP_CloseFile(HP_info *header_info);

int HP_InsertEntry(HP_info header_info, Record record);

int HP_DeleteEntry(HP_info header_info, void *value);

int HP_GetAllEntries(HP_info header_info, void *value);

#endif /*HP_H*/