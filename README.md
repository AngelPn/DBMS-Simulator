# DBMS-Simulator
A Simulation model of Database Management System in block-level and record-level.

## About

Assignment in the course "Implementation of Database Systems" at DIT - UoA. It's purpose is to implement functions that manage files created on an organizational basis of Heap Files and Hash Table Files (primary and secondary). The implementation of these functions is done above the block level that is given as a library. The implemented functions concers the creation/opening/closing of a file, the insertion/deletion of an entry (record) and the printing of records.

### Records

The Heap/Hash Table files contains records of type [`Record`](https://github.com/AngelPn/DBMS-Simulator/blob/main/include/Record.h).

### Heap File

The management of heap file is done through the functions with prefix HP_.

### Hash Table File

The management of hash table is done through the functions with prefix HT_. It is about a static hashing using the `ID` of record as key.

### Secondary Hash Table

The management of secondary hash table is done through the functions with prefix SHT_. It is about a secondary static hash table using the `surname` of record as key.

### Hash Statistics

A function [`HashStatistics`](https://github.com/AngelPn/DBMS-Simulator/blob/main/src/HashStatistics.c) is implemented to print useful statistics of hashing such as:
* the number of blocks in current file
* the minimum and the maximum number of records that a bucket has
* the mean number of records in buckets
* the mean number of blocks that a bucket has
* the number of buckets that have overflowed blocks.

## Compilation and Execution

In `src` directory, where Makefile is placed, just write command
```sh
$ make
```
Then, executables will be placed in `build` directory.
For Heap File:
```sh
$ ./main_HP
```

For Hash Table File:
```sh
$ ./main_HT
```

For Secondary Hash Table File:
```sh
$ ./main_SHT
```

## Authors

* [Theodora Panteliou](https://github.com/dora-jpg)
* [Angeliki Panagopoulou](https://github.com/AngelPn)
