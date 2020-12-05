#ifndef RECORD_H
#define RECORD_H

#define RECORD_SIZE 94

/*'struct record' is an incomplete struct*/
typedef struct record *Record;

/*Creates and returns a new record*/
Record create_record(int id, char *name, char *surname, char *address);

/*Finds the key of record determined by attrName and returns a pointer to it*/
void *get_key(Record rec, char *attrName);

/*Prints the fields of record*/
void print_record(Record rec);

/*Deallocates record*/
void free_record(Record rec);

#endif /*RECORD_H*/