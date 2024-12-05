#include <stdlib.h>
#include <stdio.h>

#include "lib/parseLib3.h"

size_t strSoftComp(char * s1, char * s2){
    size_t i = 0;
    for(; s1[i] != '\0' && s2[i] != '\0' && s1[i] == s2[i]; i++);
    return i;
}

void main(){
    char* s = NULL;
    char* s0 = NULL;
    size_t tmpLen = 0;
    int flag = 0;
    do {
        flag = 0;
        if (readLineWithDialogWithoutNewLine(&s, "Please enter main string (s).", &tmpLen)) handleMallocError();
        if (!flag && (flag = tmpLen < 2)) puts("Main string should have at least 1 charecter.");
    } while (flag);

    do {
        flag = 0;
        if (readLineWithDialogWithoutNewLine(&s0, "Please enter string to search for (s0).", &tmpLen)) handleMallocError();
        if (!flag && (flag = tmpLen < 2)) puts("String should have at least 1 charecter.");
    } while (flag);



    size_t count = 0;
    for (size_t i = 0; s[i] != '\0' ; i++){
        if(s[i] == s0[0] && strSoftComp(s + i, s0) == tmpLen - 1) count++;
    }
    puts("Result:");
    printf("%lu\n", count);

}