#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "HP.h"

int main(void){
    if (HP_CreateFile("test", 'i', "id", sizeof(int)) == 0)
        printf("success\n");
    else printf("not\n");

    HP_info *info = HP_OpenFile("test");
    printf("%d %c %s %d", info->fileDesc, info->attrType, info->attrName, info->attrLength);

    return 0;
}