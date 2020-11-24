#include <string.h>
#include <stdio.h>

#include "Record.h"

struct record
{
    int id;
    char name[15];
    char surname[25];
    char address[50];
};

void set_types(Record rec, int id, char *name, char *surname, char *address){
    rec->id = id;
    strcpy(rec->name, name);
    strcpy(rec->surname, surname);
    strcpy(rec->address, address);
}

void *get_key(Record rec, char *attrName){
    if (strcmp("id", attrName) == 0)
        return &(rec->id);
    else if(strcmp("name", attrName) == 0)
        return rec->name;
    else if(strcmp("surname", attrName) == 0)
        return rec->surname;
    else if(strcmp("address", attrName) == 0)
        return rec->address;
}

void print_record(Record rec){
    printf("ID: %d, Name: %s, Surname: %s, Address: %s\n", rec->id, rec->name, rec->surname, rec->address);
}

