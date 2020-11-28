#ifndef RECORD_H
#define RECORD_H

#define RECORD_SIZE 94


typedef struct record *Record;

Record create_record(int id, char *name, char *surname, char *address);
void *get_key(Record rec, char *attrName);
void print_record(Record rec);
void free_record(Record rec);

#endif /*RECORD_H*/