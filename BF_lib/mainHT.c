#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "HT.h"
#include "Record.h"

int main(void){
    if (HT_CreateIndex("test", 'i', "id", sizeof(int), 150) == 0)
        printf("Created index\n");
    else printf("not\n");
    HT_info *info = HT_OpenFile("test");
    if (info != NULL)
        printf("%d %c %s %d %d\n", info->fileDesc, info->attrType, info->attrName, info->attrLength, info->numBuckets);

    if (HP_CloseFile(info) < 0)
        printf("not\n");
    else printf("success\n");
}