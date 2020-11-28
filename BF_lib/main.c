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

    FILE *frecords;
    /*Open the file "records1K.txt" and read it*/
    frecords = fopen("../record_examples/records1K.txt","r");
    if (frecords == NULL){
        printf("Error: fopen() failed\n");
        exit(EXIT_FAILURE);
    }
    fseek(frecords, 0, SEEK_SET);

    int id;
    char name[15];
    char surname[25];
    char address[50];
    fscanf(frecords, "{%d,\"%[^\",\"]\",\"%[^\",\"]\",\"%[^\"]\"}", &id, name, surname, address);
    printf("ID: %d\tName: %s\tSurname: %s\tAddress: %s\n", id, name, surname, address);
    // while(fscanf(frecords, "{%d,""%s"",""%s"",""%s""}", &id, name, surname, address) != EOF){
    //     printf("ID: %d, Name: %s, Surname: %s, Address: %s\n", id, name, surname, address);
    // }

    fclose(frecords);

//     Record rec = create_record(3, "Nikos", "rgregreg", "kkkkkk");
//     //Record rec = create_record(3, "Nikos", "rgregreg", "kkkkkk");
//    // print_record(rec);


//     HP_InsertEntry(*info, rec);
    
//     if (HP_InsertEntry(*info, rec)==-1) printf("-1\n");
//     // HP_InsertEntry(*info, rec);
//     // HP_InsertEntry(*info, rec);
//     // HP_InsertEntry(*info, rec);
//     // HP_InsertEntry(*info, rec);
//     // HP_InsertEntry(*info, rec);
//     // HP_InsertEntry(*info, rec);
//     int key = 3;
//     HP_GetAllEntries(*info, &key);
//     HP_DeleteEntry(*info, &key);
//     // printf("delete\n");
//     // HP_InsertEntry(*info, rec);

//     if (HP_CloseFile(info) < 0)
//         printf("not\n");
//     else printf("success\n");

    return 0;
}