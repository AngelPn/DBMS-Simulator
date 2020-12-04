#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "headers/HT.h"
#include "headers/Record.h"

int main(void){
    if (HT_CreateIndex("test", 'i', "id", sizeof(int), 150) == 0)
        printf("Created index\n");
    else printf("not\n");
    HT_info *info = HT_OpenIndex("test");
    if (info != NULL)
        printf("%d %c %s %d %ld\n", info->fileDesc, info->attrType, info->attrName, info->attrLength, info->numBuckets);

    Record rec = create_record(5, "nikos", "ppppp", "agiou georgiou");
    HT_InsertEntry(*info, rec);
    

    if (HT_CloseIndex(info) < 0)
        printf("not\n");
    else printf("success\n");

    printf("%d\n", 150%127);
}