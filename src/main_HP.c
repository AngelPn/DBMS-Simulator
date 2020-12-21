#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/HP.h"
#include "../include/Record.h"

int main(void){

    if (HP_CreateFile("test", 'i', "id", sizeof(int)) == 0)
        printf("Created file\n");
    else printf("Error! Could not create file\n");

    HP_info *info = HP_OpenFile("test");
    if (info != NULL){
        printf("File opened with info:\n"
                "\tfileDesc: %d\n"
                "\tattrType: %c\n"
                "\tattrName: %s\n"
                "\tattrLength: %d\n",
                info->fileDesc, info->attrType, info->attrName, info->attrLength);
    }
    else printf("Error! Could not open file\n");

    FILE *frecords;
    /*Open the file "records1K.txt" and read it*/
    frecords = fopen("../examples/records1K.txt","r");
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

        if (HP_InsertEntry(*info, x)==-1){
            printf("Record with id: %d could not entry\n", id);
        }
    }

    fclose(frecords);

    int key = 54;
    printf("\nGet entry with ID = %d\n", key);
    if (HP_GetAllEntries(*info, &key) == -1)
        printf("Could not found entry\n");

    key = 107;
    printf("\nGet entry with ID = %d\n", key);
    if (HP_GetAllEntries(*info, &key) == -1)
        printf("Could not found entry\n");

    key = 803;
    printf("\nGet entry with ID = %d\n", key);
    if (HP_GetAllEntries(*info, &key) == -1)
        printf("Could not found entry\n");    

    printf("\nDelete entry with ID = %d\n", key);
    if (HP_DeleteEntry(*info, &key) == -1)
        printf("Error! Could not delete entry\n");
    printf("\nGet entry with ID = %d\n", key);
    if (HP_GetAllEntries(*info, &key) == -1)
        printf("Could not found entry\n");    

    if (HP_CloseFile(info) == 0)
        printf("\nClosed file\n");
    else printf("Error! Could not close file\n");

    return 0;
}