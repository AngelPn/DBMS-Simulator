#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "HP.h"
#include "Record.h"

int main(void){
    if (HP_CreateFile("test", 'i', "id", sizeof(int)) == 0)
        printf("success\n");
    else printf("not\n");

    HP_info *info = HP_OpenFile("test");
    printf("%d %c %s %d\n", info->fileDesc, info->attrType, info->attrName, info->attrLength);

    Record rec;
    set_types(rec, 3, "Nikos", "rgregreg", "kkkkkk");

    HP_InsertEntry(*info, rec);
    HP_InsertEntry(*info, rec);
    HP_InsertEntry(*info, rec);
    HP_InsertEntry(*info, rec);
    HP_InsertEntry(*info, rec);
    HP_InsertEntry(*info, rec);
    HP_InsertEntry(*info, rec);
    HP_InsertEntry(*info, rec);

    int count = 3;
    HP_DeleteEntry(*info, &count);
    printf("delete\n");
    HP_InsertEntry(*info, rec);

    if (HP_CloseFile(info) < 0)
        printf("not\n");
    else printf("success\n");

    return 0;
}