#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "HP.h"

int main(void){
    if (HP_CreateFile("test", 'i', "id", sizeof(int)) == 0)
        printf("success\n");
    else printf("not\n");

    return 0;
}