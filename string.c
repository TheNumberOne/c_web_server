#include "http.h"
#include <malloc.h>
#include <assert.h>
#include <memory.h>

struct string {
    char *data;
    size_t length;
    size_t capacity;
};


const int DEFAULT_STRING_LENGTH = 4;

/**
 * Creates a string. Returns null if out of memory.
 */
string_t createString() {
    string_t s = malloc(sizeof(struct string));
    if (s == NULL) return NULL;

    s->data = malloc(sizeof(char) * DEFAULT_STRING_LENGTH);
    if (s->data == NULL) return NULL;
    memset(s->data, 0, sizeof(char) * DEFAULT_STRING_LENGTH);

    s->length = 0;
    s->capacity = DEFAULT_STRING_LENGTH;
    return s;
}

char *charAt(string_t str, size_t index) {
    assert(index < str->length);
    return str->data + index;
}

/**
 * Appends the character to the string. Returns 0 on success and -1 if there is an
 * out of memory error.
 */
int append(string_t str, char c) {
    // Make sure last character is always a null character.
    if (str->length + 1 >= str->capacity) {
        size_t newLength = str->capacity * 2;
        char *newData = realloc(str->data, sizeof(char) * newLength);
        if (newData == NULL) return -1;
        memset(newData + str->capacity, 0, sizeof(char) * (newLength - str->capacity));

        str->data = newData;
        str->capacity = newLength;
    }

    str->data[str->length] = c;
    str->length++;

    return 0;
}

void destroyString(string_t string) {
    free(string->data);
    free(string);
}

void removeLastChars(string_t str, size_t n) {
    assert(str->length - n >= 0);
    str->length -= n;
    memset(str->data + str->length, 0, sizeof(char) * n);
}

char *stringData(string_t str) {
    return str->data;
}

size_t stringLength(string_t str) {
    return str->length;
}

void stringSetTo(string_t string, const char *cString) {
    size_t len = strlen(cString);
    if (len > string->capacity - 1) {
        string->data = realloc(string->data, (len + 1) * sizeof(char));
    }
    string->capacity = len + 1;
    string->length = len;
    bcopy(cString, string->data, (len + 1) * sizeof(char));
}

string_t stringFromCString(const char *cString) {
    string_t s = createString();
    stringSetTo(s, cString);
    return s;
}

string_t stringFromInt(int n) {
    string_t s = createString();
    do {
        append(s, (char) ('0' + (n % 10)));
        n /= 10;
    } while (n > 0);

    for (int i = 0; i < s->length / 2; i++) {
        char temp = s->data[i];
        s->data[i] = s->data[s->length - i - 1];
        s->data[s->length - i - 1] = temp;
    }

    return s;
}

void moveString(string_t lhs, string_t rhs) {
    free(lhs->data);
    *lhs = *rhs;
    free(rhs);
}

void plusEqual(string_t lhs, string_t rhs) {
    size_t newLength = lhs->length + rhs->length;
    if (newLength + 1 >= lhs->capacity) {
        size_t newCapacity = newLength + 1;
        lhs->data = realloc(lhs->data, newCapacity * sizeof(char));
        lhs->capacity = newCapacity;
    }

    memcpy(lhs->data + lhs->length, rhs->data, (rhs->length + 1) * sizeof(char));
    lhs->length = newLength;
}

string_t stringCopy(string_t s) {
    string_t ret = createString();
    plusEqual(ret, s);
    return ret;
}

void writeString(int fd, string_t str) {
    write(fd, stringData(str), stringLength(str));
}