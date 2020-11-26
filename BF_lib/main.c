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

    Record rec = create_record(3, "Nikos", "rgregreg", "kkkkkk");
    //Record rec = create_record(3, "Nikos", "rgregreg", "kkkkkk");
   // print_record(rec);

    HP_InsertEntry(*info, rec);
    
    if (HP_InsertEntry(*info, rec)==-1) printf("-1\n");
    // HP_InsertEntry(*info, rec);
    // HP_InsertEntry(*info, rec);
    // HP_InsertEntry(*info, rec);
    // HP_InsertEntry(*info, rec);
    // HP_InsertEntry(*info, rec);
    // HP_InsertEntry(*info, rec);
    int key = 3;
    HP_GetAllEntries(*info, &key);
    HP_DeleteEntry(*info, &key);
    // printf("delete\n");
    // HP_InsertEntry(*info, rec);

    if (HP_CloseFile(info) < 0)
        printf("not\n");
    else printf("success\n");

    return 0;
}