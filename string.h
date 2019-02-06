//
// Created by thenumberone on 1/31/19.
//

#ifndef PROJECT2_STRING_H
#define PROJECT2_STRING_H

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

void stringSetTo(string_t string, char* cString);

string_t stringFromCString(char* cString);


string_t stringFromInt(int n);

#endif //PROJECT2_STRING_H
