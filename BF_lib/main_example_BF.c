#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "BF.h"

#define FILENAME "file"
#define MAX_FILES 100
#define MAX_BLOCKS 500

int main(int argc, char** argv) {
	int bfs [MAX_FILES];
	int i, j;
	char filename [5];
	void* block;
	int blkCnt;

	BF_Init();
	strcpy(filename, FILENAME);
	for (i = 0; i < MAX_FILES; i++) {
		printf("File %s\n", filename);
		if (BF_CreateFile(filename) < 0) {
			BF_PrintError("Error creating file");
			exit(EXIT_FAILURE);
		}
		if ((bfs[i] = BF_OpenFile(filename)) < 0) {
			BF_PrintError("Error opening file");
			break;
		}
		for (j = 0; j < MAX_BLOCKS; j++) {

			printf("Block %d\n", j);
			if (BF_AllocateBlock(bfs[i]) < 0) {
				BF_PrintError("Error allocating block");
				break;
			}

			blkCnt = BF_GetBlockCounter(bfs[i]);
			printf("File %d has %d blocks\n", bfs[i], blkCnt);

			if (BF_ReadBlock(bfs[i], j, &block) < 0) {
				BF_PrintError("Error getting block");
				break;
			}
			strncpy(block, (char*)&j, sizeof(int));
			if (BF_WriteBlock(bfs[i], j) < 0){
				BF_PrintError("Error writing block back");
				break;
			}
		}
		filename[0]++;

		if (BF_CloseFile(bfs[i]) < 0) {
			BF_PrintError("Error closing file");
			break;
		}

	}
	return 0;
}

