#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "headers/HP.h"
#include "headers/Record.h"

int main(void){
    if (HP_CreateFile("test", 'i', "id", sizeof(int)) == 0)
        printf("Created file\n");
    else printf("not\n");

    HP_info *info = HP_OpenFile("test");
    if (info != NULL)
        printf("%d %c %s %d\n", info->fileDesc, info->attrType, info->attrName, info->attrLength);

    FILE *frecords;
    /*Open the file "records1K.txt" and read it*/
    frecords = fopen("../record_examples/records1K.txt","r");
    if (frecords == NULL){
        printf("Error: fopen() failed\n");
        exit(EXIT_FAILURE);
    }
    //fseek(frecords, 0, SEEK_SET);
    
    int id;
    char name[15];
    char surname[25];
    char address[50];
    Record x;
    int countiter;
    while(fscanf(frecords, "{%d,\"%[^\",\"]\",\"%[^\",\"]\",\"%[^\"]\"}\n", &id, name, surname, address) != EOF){
        x = create_record(id, name, surname, address);
        if (HP_InsertEntry(*info ,x)==-1){
            printf("Record with id: %d could not entry\n", id);
        }
        free_record(x);
        if (countiter > 100) break;
        countiter++;
    }

    fclose(frecords);

    int key = 54;
    HP_GetAllEntries(*info, &key);
    HP_DeleteEntry(*info, &key);

    if (HP_CloseFile(info) < 0)
        printf("not\n");
    else printf("success\n");

    return 0;
}