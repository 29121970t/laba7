#include <limits.h>
#include <stdio.h>

#include "lib/parseLib3.h"

size_t strLen(char* str) {
    size_t i = 0;
    for (; str[i] != '\0'; i++);
    return i + 1;
}

size_t getWordOffsetByIndex(char* str, size_t wordIndex, size_t* wordOffset, size_t* length) {
    size_t cursor = 0;
    size_t wordLen = 0;
    size_t i = 0;

    for (; cursor < ULONG_MAX && isSpace(str[cursor]); cursor++);
    for (; i < wordIndex && str[cursor] != '\0'; i++) {
        if (cursor == ULONG_MAX) return ERTOOLONG;

        wordLen = 0;
        for (; !isSpace(str[cursor + wordLen]) && str[cursor + wordLen] != '\0'; wordLen++);
        cursor += wordLen + 1;
        for (; cursor < ULONG_MAX && isSpace(str[cursor]); cursor++);
    }
    if (str[cursor] == '\0') return EROUTOFRANGE;
    if (wordOffset) *wordOffset = cursor;
    if (length) *length = wordLen;

    return 0;
}
size_t changeWordByOffset(char* str, size_t wordOffset, char* word) {
    size_t strLength = strLen(str);
    size_t newWordLength = strLen(word) - 1;
    size_t oldWordLen = 0;

    for (; !isSpace(str[wordOffset + oldWordLen]) && str[wordOffset + oldWordLen] != '\0'; oldWordLen++);

    long long delta = newWordLength - oldWordLen;
    if (delta < 0) {
        for (size_t j = wordOffset; j < strLength + delta; j++) {
            str[j] = str[j - delta];
        }
        if ((str = realloc(str, (strLength + delta) * sizeof(char))) == NULL) return ERMALLOC;
    }
    if (delta > 0) {
        if ((str = realloc(str, (strLength + delta) * sizeof(char))) == NULL) return ERMALLOC;
        for (size_t j = strLength + delta; j > wordOffset; --j) {
            str[j] = str[j - delta];
        }
    }

    for (size_t k = 0; k < newWordLength; k++) {
        str[wordOffset + k] = word[k];
    }

    return 0;
}

int isWord(char* str) {
    for (size_t i = 0; str[i] != '\0'; i++) {
        if (isSpace(str[i])) return 0;
    }
    return 1;
}

void main() {
    char* inputStr = NULL;
    char* word1 = NULL;
    char* word2 = NULL;

    long k = 0;
    long m = 0;

    size_t offset1 = 0;
    size_t offset2 = 0;
    int flag = 0;
    size_t inputStrLength = 0;

    if (readLineWithDialog(&inputStr, "Please enter main string.", &inputStrLength)) handleMallocError();
    inputStr[inputStrLength - 2] = '\0';

    do {
        flag = 0;
        size_t wordLength = 0;

        if (readLineWithDialog(&word1, "Please enter first word.", &wordLength)) handleMallocError();
        word1[wordLength - 2] = '\0';

        if (!flag && (flag = !isWord(word1))) puts("Got not a word. (Remove spaces.)");
    } while (flag);

    do {
        flag = 0;
        size_t wordLength = 0;
        if (readLongWithDialog(&k, "Please enter first Index k. (A number > 0)")) handleMallocError();
        if (flag = k <= 0) puts("Number should be > 0");
        if (!flag && (flag = getWordOffsetByIndex(inputStr, k - 1, &offset1, NULL))) puts("Main string has less words then provided number.");
    } while (flag);

    if (changeWordByOffset(inputStr, offset1, word1)) handleMallocError();


    do {
        flag = 0;
        size_t wordLength = 0;
        if (readLineWithDialog(&word2, "Please enter second word.", &wordLength)) handleMallocError();
        word2[wordLength - 2] = '\0';
        if (flag = !isWord(word2)) puts("Got not a word. (Remove spaces.)");
    } while (flag);

    do {
        flag = 0;
        size_t wordLength = 0;
        if (readLongWithDialog(&m, "Please enter second Index m. (A number > 0)")) handleMallocError();
        if (flag = m <= 0) puts("Number should be > 0");
        if (m == k) puts("WARNING: m = k. The k-th word will be replaced with <second word>");
        if (!flag && (flag = getWordOffsetByIndex(inputStr, m - 1, &offset2, &wordLength))) puts("Main string has less words then provided number.");

    } while (flag);

    if (changeWordByOffset(inputStr, offset2, word2)) handleMallocError();

    puts("Result:");
    printf("%s\n", inputStr);

    free(inputStr);
    free(word1);
    free(word2);
}
