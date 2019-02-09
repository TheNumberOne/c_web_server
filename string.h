#pragma once
#include <stddef.h>

struct string;
typedef struct string *string_t;

string_t createString();

char *charAt(string_t str, size_t index);

char* stringData(string_t str);

size_t stringLength(string_t str);

int append(string_t str, char c);

void removeLastChars(string_t str, size_t n);

void destroyString(string_t string);

void stringSetTo(string_t string, const char *cString);

string_t stringFromCString(const char *cString);

string_t stringFromInt(int n);

void plusEqual(string_t lhs, string_t rhs);

string_t stringCopy(string_t s);

/**
 * Destroys \p rhs and puts its contents into lhs
 * @param lhs
 * @param rhs
 */
void moveString(string_t lhs, string_t rhs);

