#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/HT.h"
#include "../include/Record.h"
#include "../include/HashStatistics.h"

int main(void){

    if (HT_CreateIndex("test", 'i', "id", sizeof(int), 409) == 0)
        printf("Created file\n");
    else printf("Error! Could not create file\n");

    HT_info *info = HT_OpenIndex("test");
    if (info != NULL){
        printf("File opened with info:\n"
                "\tfileDesc: %d\n"
                "\tattrType: %c\n"
                "\tattrName: %s\n"
                "\tattrLength: %d\n"
                "\tnumBuckets: %ld\n",
                info->fileDesc, info->attrType, info->attrName, info->attrLength, info->numBuckets);
    }
    else printf("Error! Could not open file\n");

    FILE *frecords;
    /*Open the file "records1K.txt" and read it*/
    frecords = fopen("../examples/records5K.txt","r");
    if (frecords == NULL){
        printf("Error: fopen() failed\n");
        exit(EXIT_FAILURE);
    }
    
    /*Insert every record of file in heap*/
    int id;
    char name[15];
    char surname[25];
    char address[50];
    Record x;
    while(fscanf(frecords, "{%d,\"%[^\",\"]\",\"%[^\",\"]\",\"%[^\"]\"}\n", &id, name, surname, address) != EOF){
        init_record(&x, id, name, surname, address);
        if (HT_InsertEntry(*info, x)==-1){
            printf("Record with id: %d could not entry\n", id);
        }
    }

    fclose(frecords);

    if (HT_GetAllEntries(*info, NULL) == -1) /*print all entries in file*/
        printf("Could not find entry\n");

    int key = 21;
    printf("\nGet entry with ID = %d\n", key);
    if (HT_GetAllEntries(*info, &key) == -1)
        printf("Could not find entry\n");

    key = 107;
    printf("\nGet entry with ID = %d\n", key);
    if (HT_GetAllEntries(*info, &key) == -1)
        printf("Could not find entry\n");

    key = 803;
    printf("\nGet entry with ID = %d\n", key);
    if (HT_GetAllEntries(*info, &key) == -1)
        printf("Could not find entry\n");    

    key = 8003;
    printf("\nGet entry with ID = %d\n", key);
    if (HT_GetAllEntries(*info, &key) == -1)
        printf("Could not find entry\n"); 

    key = 13078;
    printf("\nGet entry with ID = %d\n", key);
    if (HT_GetAllEntries(*info, &key) == -1)
        printf("Could not find entry\n"); 

    printf("\nDelete entry with ID = %d\n", key);
    if (HT_DeleteEntry(*info, &key) == -1)
        printf("Error! Could not delete entry\n");
    printf("\nGet entry with ID = %d\n", key);
    if (HT_GetAllEntries(*info, &key) == -1)
        printf("Could not find entry\n");  

    printf("\nHash Statistics:\n");
    HashStatistics("test");
    
    if (HT_CloseIndex(info) == 0)
        printf("\nClosed file\n");
    else printf("Error! Could not close file\n");
    
    return 0;
}