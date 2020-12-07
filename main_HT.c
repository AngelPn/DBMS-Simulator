#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/HT.h"
#include "include/Record.h"

int main(void){
    if (HT_CreateIndex("test", 'i', "id", sizeof(int), 150) == 0)
        printf("Created index\n");
    else printf("not\n");
    HT_info *info = HT_OpenIndex("test");
    if (info != NULL)
        printf("%d %c %s %d %ld\n", info->fileDesc, info->attrType, info->attrName, info->attrLength, info->numBuckets);

    FILE *frecords;
    /*Open the file "records1K.txt" and read it*/
    frecords = fopen("./examples/records1K.txt","r");
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
        if (HT_InsertEntry(*info ,x)==-1){
            printf("Record with id: %d could not entry\n", id);
        }
        free_record(x);
        if (countiter > 1000) break;
        countiter++;
    }

    fclose(frecords);

    int key = 54;
    HT_GetAllEntries(*info, &key);
    HT_DeleteEntry(*info, &key);

    HashStatistics("test");
    
    if (HT_CloseIndex(info) < 0)
        printf("not\n");
    else printf("success\n");
    

}