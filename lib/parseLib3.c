#include "parseLib3.h"

enum { ERMALLOC = 1, EROUTOFRANGE, ERTOOLONG, ERINVALIDARG, ERINVALIDUERINPUT, ERUNKNOWN };

void handleMallocError() {
    puts("MALLOC ERROR.");
    exit(1);
}

int isDigit(char c) { return c >= '0' && c <= '9' ? 1 : 0; }
int isValidIntChar(char c) { return isDigit(c) || c == '-'; }
int isSpace(char c) { return c == ' ' || c == '\t' || c == '\n'; }
int isValidDoubleChar(char c) { return isValidIntChar(c) || c == '.'; }
int isValidDoubleStringChar(char c) { return isValidDoubleChar(c) || isSpace(c); }
int isValidIntString(char c) { return isValidIntChar(c) || isSpace(c); }

int isValidString(char* str, int (*validator)(char)) {
    for (size_t i = 0; i < ULONG_MAX; i++) {
        if (str[i] == '\0') return 1;
        if (!validator(str[i])) return 0;
    }
    return 1;
}

int isValidString_l(char* str, size_t length, int (*validator)(char)) {
    for (size_t i = 0; i < length; i++) {
        if (!validator(str[i])) return 0;
    }
    return 1;
}

int isValidString_c(char* str, int (*validator)(char), char c) {
    for (size_t i = 0; i < ULONG_MAX; i++) {
        if (str[i] == '\0') return 1;
        if (!validator(str[i]) && str[i] != c) return 0;
    }
    return 1;
}

size_t getLine(char** string, size_t* size, FILE* stream) {
    size_t currentSize = 8;
    size_t counter;
    char* result = NULL;

    if ((result = calloc(currentSize, sizeof(char))) == NULL) return ERMALLOC;

    for (counter = 0; counter < ULONG_MAX; counter++) {
        if (currentSize - counter == 0) {
            if ((result = realloc(result, currentSize * 2 * sizeof(char))) == NULL) return ERMALLOC;
            currentSize *= 2;
        }

        result[counter] = getc(stream);
        if (result[counter] == '\n') break;
    }
    if (counter == ULONG_MAX) return ERTOOLONG;
    if ((result = realloc(result, (counter + 2) * sizeof(char))) == NULL) return ERMALLOC;
    currentSize = counter + 2;
    *string = result;
    if (size != NULL) {
        *size = currentSize;
    }
    return 0;
}

long parseLong(char* startPtr, char** endPtr) {
    long result = 0;
    unsigned cursor = 0;
    int negFlag = 0;
    unsigned digitCounter = 0;

    for (; cursor < UINT_MAX; cursor++) {
        if (!isSpace(startPtr[cursor])) break;
    }

    if (startPtr[cursor] == '-') {
        negFlag = 1;
        cursor++;
    }

    for (digitCounter = cursor; digitCounter < UINT_MAX; digitCounter++) {
        if (!isDigit(startPtr[digitCounter])) {
            if (digitCounter == cursor) errno = EINVAL;
            if (endPtr != NULL) *endPtr = startPtr + digitCounter;
            return negFlag ? -result : result;
        }

        long testMult = result * 10;
        int digit = startPtr[digitCounter] - '0';

        if (testMult / 10 != result || LONG_MAX - testMult < digit) {
            errno = ERANGE;
            if (endPtr != NULL) *endPtr = startPtr + digitCounter;
            return negFlag ? LONG_MIN : LONG_MAX;
        }
        result = testMult + digit;
    }

    if (digitCounter == UINT_MAX) {
        errno = ERANGE;
    }
    return 0;
}

size_t readLong(long* ptr) {
    char* str = NULL;
    size_t errorTmp = 0;
    if (errorTmp = getLine(&str, NULL, stdin)) return errorTmp;
    if (!isValidString(str, isValidIntString)) {free(str); return ERINVALIDUERINPUT;}

    *ptr = parseLong(str, NULL);
    free(str);
    if (errno == ERANGE) return EROUTOFRANGE;
    if (errno == EINVAL) return ERINVALIDUERINPUT;
    return 0;
}

size_t parseMultLong(long** array, char separator, char* inputString, size_t* count) {
    if (separator == '.') return ERINVALIDARG;
    if (!isValidString_c(inputString, isValidIntString, separator)) return ERINVALIDUERINPUT;

    char* tmpPtr1 = inputString;
    char* tmpPtr2 = NULL;

    size_t bufferSize = 8;
    size_t counter = 0;
    size_t errorTmp = 0;
    long* result = NULL;

    if ((result = calloc(bufferSize, sizeof(long))) == NULL) return ERMALLOC;

    for (counter = 0; counter < ULONG_MAX; counter++) {
        if (bufferSize == counter) {
            if ((result = realloc(result, bufferSize * 2 * sizeof(long))) == NULL) return ERMALLOC;
            bufferSize *= 2;
        }
        errno = 0;
        result[counter] = parseLong(tmpPtr1, &tmpPtr2);

        unsigned i = 0;
        for (; i < UINT_MAX; i++) {
            if (!isSpace(tmpPtr2[i])) break;
        }

        if (errno || (tmpPtr2[i] != separator)) {
            if ((*array = realloc(result, (counter + 1) * sizeof(long))) == NULL) {
                *count = bufferSize;
                return ERMALLOC;
            }
            *count = counter + 1;
            if (errno == ERANGE) return EROUTOFRANGE;
            if (errno == EINVAL) return ERINVALIDUERINPUT;
            return 0;
        }

        tmpPtr1 = tmpPtr2 + 1 + i;
    }

    if (counter == ULONG_MAX) return ERTOOLONG;

    if ((*array = realloc(result, (counter + 1) * sizeof(long))) == NULL) {
        *count = bufferSize;
        return ERMALLOC;
    }
    *count = counter + 1;

    return 0;
}

size_t readMultLong(long** array, char separator, size_t* count) {
    char* str = NULL;
    size_t tmpError = 0;
    if (tmpError = getLine(&str, NULL, stdin)) return tmpError;
    if (tmpError = parseMultLong(array, separator, str, count)) {free(str);return tmpError;}
    free(str);
    return 0;
}

size_t readMultLongWithDialog(long** array, char separator, const char* massege, size_t* count) {
    size_t tmpError = 0;
    do {
        puts(massege);
        if (tmpError = readMultLong(array, separator, count)) {
            puts("Error occured.");

            if (tmpError == EROUTOFRANGE) puts("Some Number is out of <long int> range");
            if (tmpError == ERTOOLONG) puts("Some Number has too many digits");
            if (tmpError == ERINVALIDUERINPUT) puts("Input format error. Reread description and try again.");


            if (tmpError == ERMALLOC) return ERMALLOC;

        }

    } while (tmpError);
    return 0;
}

size_t readLongWithDialog(long* ptr, char * messege){
    size_t tmpError = 0;
    do {
        puts(messege);
        if (tmpError = readLong(ptr)) {
            puts("Error occured.");

            if (tmpError == EROUTOFRANGE) puts("Number is out of <long int> range");
            if (tmpError == ERTOOLONG) puts("Number has too many digits");
            if (tmpError == ERINVALIDUERINPUT) puts("Input format error. Reread description and try again.");


            if (tmpError == ERMALLOC) return ERMALLOC;

        }

    } while (tmpError);
    return 0;
}


double parseDouble(char* startPtr, char** endPtr) {
    double result = 0;
    double shift = .1;
    unsigned cursor = 0;
    int negFlag = 0;
    unsigned digitCounter = 0;
    long dotPos = -1;

    for (; cursor < UINT_MAX; cursor++) {
        if (!isSpace(startPtr[cursor])) break;
    }

    if (startPtr[cursor] == '-') {
        negFlag = 1;
        cursor++;
    }

    for (digitCounter = cursor; digitCounter < UINT_MAX; digitCounter++) {
        if (!isDigit(startPtr[digitCounter])) {
            if (digitCounter == cursor) errno = EINVAL;
            if (dotPos == -1 && startPtr[digitCounter] == '.') {
                dotPos = digitCounter;
                continue;
            }
            if (endPtr != NULL) *endPtr = startPtr + digitCounter;
            break;
        }

        double testMult = result * 10;
        double digit = startPtr[digitCounter] - '0';

        result = testMult + digit;
    }
    if (digitCounter == INT_MAX) {
        errno = ERANGE;
        return 0;
    }

    if (dotPos != -1) {
        for (long i = dotPos + 1; i < digitCounter; i++) {
            result /= 10;
        }
    }

    return negFlag ? -result : result;
}

size_t readDouble(double* ptr) {
    char* str = NULL;
    size_t errorTmp = 0;
    if (errorTmp = getLine(&str, NULL, stdin)) { return errorTmp;}
    if (!isValidString(str, isValidDoubleStringChar)) {free(str); return ERINVALIDUERINPUT;}

    *ptr = parseLong(str, NULL);
    free(str);
    if (errno == ERANGE) return EROUTOFRANGE;
    if (errno == EINVAL) return ERINVALIDUERINPUT;
    
    return 0;
}

size_t parseMultDouble(double** array, char separator, char* inputString, size_t * count) {
    if (separator == '.') return ERINVALIDARG;
    if (!isValidString_c(inputString, isValidDoubleStringChar, separator)) return ERINVALIDUERINPUT;

    char* tmpPtr1 = inputString;
    char* tmpPtr2 = NULL;

    size_t bufferSize = 8;
    size_t counter;
    size_t errorTmp = 0;

    double* result = NULL;

    if ((result = calloc(bufferSize, sizeof(double))) == NULL) return ERMALLOC;
    for (counter = 0; counter < ULONG_MAX; counter++) {
        if (bufferSize == counter) {
            if ((result = realloc(result, bufferSize * 2 * sizeof(double))) == NULL) return ERMALLOC;
            bufferSize *= 2;
        }
        errno = 0;
        result[counter] = parseDouble(tmpPtr1, &tmpPtr2);

        unsigned i = 0;
        for (; i < UINT_MAX; i++) {
            if (!isSpace(tmpPtr2[i])) break;
        }

        if (errno || (tmpPtr2[i] != separator)) {
            if ((*array = realloc(result, (counter + 1) * sizeof(double))) == NULL) {
                *count = bufferSize;
                return ERMALLOC;
            }
            *count = counter + 1;
            if (errno == ERANGE) return EROUTOFRANGE;
            if (errno == EINVAL) return ERINVALIDUERINPUT;
            return 0;
        }

        tmpPtr1 = tmpPtr2 + 1 + i;
    }
    

    if (counter == ULONG_MAX) return ERTOOLONG;

    if ((*array = realloc(result, (counter + 1) * sizeof(double))) == NULL) {
        *count = bufferSize;
        return ERMALLOC;
    }
    *count = counter + 1;

    return 0;
}

size_t readMultDouble(double** array, char separator, size_t * count) {
    char* str = NULL;
    size_t tmpError = 0;
    if (tmpError = getLine(&str, NULL, stdin))  return tmpError;
    if (tmpError = parseMultDouble(array, separator, str, count)) {free(str);return tmpError;}
    free(str);
    return 0;
}

size_t readMultDoubleWithDialog(double** array, char separator, const char* massege, size_t * count) {

    size_t tmpError = 0;
    do {
        puts(massege);
        if (tmpError = readMultDouble(array, separator, count)) {
            puts("Error occured.");

            if (tmpError == EROUTOFRANGE) puts("Some Number is out of <long int> range");
            if (tmpError == ERTOOLONG) puts("Some Number has too many digits");
            if (tmpError == ERINVALIDUERINPUT) puts("Input format error. Reread description and try again.");


            if (tmpError == ERMALLOC) return ERMALLOC;

        }

    } while (tmpError);
    return 0;
}

size_t readDoubleWithDialog(double* ptr, char * messege){
    size_t tmpError = 0;
    do {
        puts(messege);
        if (tmpError = readDouble(ptr)) {
            puts("Error occured.");

            if (tmpError == EROUTOFRANGE) puts("Number is out of <long int> range");
            if (tmpError == ERTOOLONG) puts("Number has too many digits");
            if (tmpError == ERINVALIDUERINPUT) puts("Input format error. Reread description and try again.");


            if (tmpError == ERMALLOC) return ERMALLOC;
        }

    } while (tmpError);
    return 0;
}


size_t readLine(char** string, size_t* size){
    size_t errorTmp = 0;
    if (errorTmp = getLine(string, size, stdin)) { return errorTmp;}    
    return 0;
}
//some v1 junk
size_t readYNWithDialog(const char* str, int * res) {
    size_t errorTmp = 0;
    char * answ1 = NULL;
    int flag = 0;
    do {
        flag = 0;
        puts(str);
        if (errorTmp = readLine(&answ1, NULL)) { return errorTmp;}  

        if (answ1[0] != 'Y' && answ1[0] != 'N' && answ1[0] != 'y' && answ1[0] != 'n' || answ1[1] != '\n') {
            flag = 1;
            puts("ERROR: Cannont interpret this answer. Plese try again.");
        }

    } while (flag);
    *res = answ1[0] == 'Y' || answ1[0] == 'y';
    return 0;
}

size_t readUDWithDialog(const char* str, int * res) {
    size_t errorTmp = 0;
    char * answ1 = NULL;
    int flag = 0;
    do {
        flag = 0;
        puts(str);
        if (errorTmp = readLine(&answ1, NULL)) { return errorTmp;}  

        if (answ1[0] != 'U' && answ1[0] != 'D' && answ1[0] != 'u' && answ1[0] != 'd' || answ1[1] != '\n') {
            flag = 1;
            puts("ERROR: Cannont interpret this answer. Plese try again.");
        }

    } while (flag);
    *res = answ1[0] == 'Y' || answ1[0] == 'y';
    return 0;
}

void fill2DArrayByHand(double** inputArray, unsigned rowCount, unsigned columnCount, int (*validator)(double)) {
    char tmpStr[100] = {0};
    for (long i = 0; i < rowCount; i++) {
        int passFlag = 0;
        do {
            passFlag = 0;
            size_t count = 0;
            sprintf(tmpStr, "Please enter %ld-th row: %u numbers > 0.", i + 1, columnCount);
            if (readMultDoubleWithDialog(inputArray + i, ',', tmpStr, &count)) handleMallocError();

            for (size_t k = 0; k < count; k++) {
                if (validator && validator(inputArray[i][k])) {passFlag = 1; break;}
            }

            if (count != columnCount) {
                passFlag = 1;
                puts("Number count mismatch. Check your input and try again.");
                continue;
            }

            if (passFlag) puts("Got number <= 0. Check your input and try again.");
        } while (passFlag);
    }
}

void fill2DArrayWithRandom(double** inputArray, unsigned rowCount, unsigned columnCount, double from, double to) {
    long seed = 0;
    do {
        if (readLongWithDialog(&seed, "Please enter seed for random number generation (positive number < LONG_MAX)")) handleMallocError();
        if (seed <= 0) puts("Seed should be greater then 0. Please try again.");
    } while (seed <= 0);

    unsigned rand = MrandomUInt(seed);
    for (long i = 0; i < rowCount; i++) {
        if ((inputArray[i] = calloc(columnCount, sizeof(long))) == NULL) handleMallocError();

        for (int j = 0; j < columnCount; j++) {
            rand = MrandomUInt(rand);
            inputArray[i][j] = map(0, 254803967, from, to, rand);
        }
    }
}
