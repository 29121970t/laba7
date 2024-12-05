#include <stdio.h>
#include <stdlib.h>

#include "lib/parseLib3.h"

void swapLong(long* a, long* b) {
    long tmp = *a;
    *a = *b;
    *b = tmp;
}
void swapPointer(char** a, char** b){
    char *tmp = *a;
    *a = *b;
    *b = tmp;
}
long _subdevide(long* array, char ** stringArray, long a, long b) {
    long deviderValue = array[(a + b) / 2];
    long i = a;
    long j = b;
    while (i <= j) {
        for (; array[i] < deviderValue; i++);
        for (; array[j] > deviderValue; j--);

        if (i >= j) break;
        swapLong(&array[i++], &array[j--]);
        swapPointer(&stringArray[i], &stringArray[j]);
    }
    return j;
}
void horSort(long* array, char ** stringArray, long l, long h) {
    if (l < h) {
        long ptr = _subdevide(array, stringArray, l, h);
        horSort(array, stringArray,  l, ptr);
        horSort(array, stringArray, ptr + 1, h);
    }
}
void printArray(char** inputArray, long rowCount) {
    for (size_t i = 0; i < rowCount; i++) {
        printf("| ");
        printf("%s, ", inputArray[i]);
        printf("|\n");
    }
}

int isvalidateString(char* str) {
    for (size_t i = 0; str[i] != '\0'; i++) {
        if (!isSpace(str[i])) return 1;
    }
    return 0;
}

void main() {
    long size = 0;
    char** array = NULL;
    do {
        if (readLongWithDialog(&size, "Please enter string count (positive number < LONG_MAX)")) handleMallocError();
        if (size <= 0) puts("String count  be greater then 0. Please try again.");
    } while (size <= 0);

    if ((array = calloc(size, sizeof(char*))) == NULL) handleMallocError();

    char tmpStr[100] = {0};
    for (long i = 0; i < size; i++) {
        int flag = 0;
        do {
            flag = 0;
            sprintf(tmpStr, "Please enter %ld-th string", i + 1);
            if (readLineWithDialogWithoutNewLine(array + i, tmpStr, NULL)) handleMallocError();
            if (flag = !isvalidateString(array[i])) puts("String should have at least 1 non-space charecter");

        } while (flag);
    }

    size_t* charCounts = NULL;

    if ((charCounts = calloc(size, sizeof(size_t*))) == NULL) handleMallocError();

    for (long i = 0; i < size; i++) {
        size_t cursor = 0;
        size_t wordLen = 0;
        

        for (; cursor < isSpace(array[i][cursor]); cursor++);
        for (size_t j = 0; array[i][cursor] != '\0'; j++) {

            wordLen = 0;
            for (; !isSpace(array[i][cursor + wordLen]) && array[i][cursor + wordLen] != '\0'; wordLen++);
            if (wordLen % 2 == 0) charCounts[i]++;
            cursor += wordLen + 1;
            for (; cursor < isSpace(array[i][cursor]); cursor++);
        }
    }


    horSort(charCounts, array, 0, size - 1);

    puts("Result:");
    printArray(array, size);
}