#ifndef RECORD_H
#define RECORD_H

#define RECORD_SIZE 94

typedef struct record
{
    int id;
    char name[15];
    char surname[25];
    char address[50];
} Record;

/*Initializes record*/
void init_record(Record *rec, int id, char *name, char *surname, char *address);

/*Finds the key of record determined by attrName and returns a pointer to it*/
void *get_key(Record *rec, char *attrName);

/*Prints the fields of record*/
void print_record(Record rec);

#endif /*RECORD_H*/