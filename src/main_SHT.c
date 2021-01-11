#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/HT.h"
#include "../include/SHT.h"
#include "../include/Record.h"

int main(void){

    /*Create Hash File*/
    if (HT_CreateIndex("test", 'i', "id", sizeof(int), 150) == 0)
        printf("Created file\n");
    else printf("Error! Could not create file\n\n");

    /*Create Secondary Hash File*/
    if (SHT_CreateSecondaryIndex("secondary_test", "surname", sizeof(char)*25, 150, "test") == 0)
        printf("Created Secondary Hash file\n\n");
    else printf("Error! Could not create file\n");

    /*Open Hash File*/
    HT_info *ht_info = HT_OpenIndex("test");
    if (ht_info != NULL){
        printf("Hash File opened with info:\n"
                "\tfileDesc: %d\n"
                "\tattrType: %c\n"
                "\tattrName: %s\n"
                "\tattrLength: %d\n"
                "\tnumBuckets: %ld\n\n",
                ht_info->fileDesc, ht_info->attrType, ht_info->attrName, ht_info->attrLength, ht_info->numBuckets);
    }
    else printf("Error! Could not open file\n");

    /*Open Secondary Hash File*/
    SHT_info *sht_info = SHT_OpenSecondaryIndex("secondary_test");
    if (sht_info != NULL){
        printf("Secondary Hash File opened with info:\n"
                "\tfileDesc: %d\n"
                "\tfileName %s\n"
                "\tattrName: %s\n"
                "\tattrLength: %d\n"
                "\tnumBuckets: %ld\n\n",
                sht_info->fileDesc, sht_info->fileName, sht_info->attrName, sht_info->attrLength, sht_info->numBuckets);
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

    int blockID;
    while(fscanf(frecords, "{%d,\"%[^\",\"]\",\"%[^\",\"]\",\"%[^\"]\"}\n", &id, name, surname, address) != EOF){
        init_record(&x, id, name, surname, address);
        if ((blockID = HT_InsertEntry(*ht_info, x))==-1){
            printf("Record with id: %d could not entry Hash File\n", id);
        }
        
        // SecondaryRecord srec = {.record.id = x.id,
        //                         .record.name = x.name,
        //                         .record.surname = x.surname,
        //                         .record.address = x.address,
        //                         .blockId = blockID
        //                         };
        SecondaryRecord srec = { .blockId = blockID };
        init_record(&srec.record, x.id, x.name, x.surname, x.address);
        if(SHT_SecondaryInsertEntry(*sht_info, srec) == -1){
            printf("Record with surname: %s could not entry Secondary Hash File\n", x.surname);
        }
    }

    fclose(frecords);
    // if (HT_GetAllEntries(*ht_info, NULL) == -1)
    //     printf("Could not find entry\n");

    char key[25];
    strcpy(key,"surname_2");
    printf("\nGet entry with = %s\n", key);
    if (SHT_SecondaryGetAllEntries(*sht_info,*ht_info, &key) == -1)
        printf("Could not find entry\n");

    strcpy(key,"surname_7");
    printf("\nGet entry with = %s\n", key);
    if (SHT_SecondaryGetAllEntries(*sht_info,*ht_info, &key) == -1)
        printf("Could not find entry\n");

    strcpy(key,"surname_14");
    printf("\nGet entry with = %s\n", key);
    if (SHT_SecondaryGetAllEntries(*sht_info,*ht_info, &key) == -1)
        printf("Could not find entry\n");    

    strcpy(key,"surname_10");
    printf("\nGet entry with = %s\n", key);
    if (SHT_SecondaryGetAllEntries(*sht_info,*ht_info, &key) == -1)
        printf("Could not find entry\n"); 

    printf("\nGet statistics of SHT file\n");
    SHT_HashStatistics("secondary_test");

    SHT_CloseSecondaryIndex(sht_info);
    HT_CloseIndex(ht_info);
    return 0;
}