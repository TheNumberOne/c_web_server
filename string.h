#pragma once

#include <stddef.h>

/**
 * Represents a simple string structure
 * I found this personally a bit easier to use than c strings.
 */
struct string;
typedef struct string *string_t;

string_t createString();

/**
 * Returns a pointer to the char at the specified index.
 */
char *charAt(string_t str, size_t index);

/**
 * Returns the null terminated data of the string.
 */
char *stringData(string_t str);

/**
 * Returns the length of this string, not including trailing
 * null characters.
 */
size_t stringLength(string_t str);

/**
 * Appens the specified character to the string
 */
int append(string_t str, char c);

/**
 * Removes the specified number of characters from the string.
 */
void removeLastChars(string_t str, size_t n);

void destroyString(string_t string);

/**
 * Sets contents of the string to the contents of the c string.
 * @param cString not taken ownership
 */
void stringSetTo(string_t string, const char *cString);

/**
 * Creates a new string with contents from the c string
 * @param cString not taken ownership
 */
string_t stringFromCString(const char *cString);

/**
 * Creates a new string with contents of the string form of an integer
 */
string_t stringFromInt(int n);

/**
 * Appends \p rhs to \p lhs.
 * @param rhs not taken ownership
 */
void plusEqual(string_t lhs, string_t rhs);

/**
 * Copies the specified string.
 */
string_t stringCopy(string_t s);

/**
 * Destroys \p rhs and puts its contents into lhs
 * @param lhs
 * @param rhs
 */
void moveString(string_t lhs, string_t rhs);

/**
 * Writes the specified string to the output file decriptor
 */
void writeString(int fd, string_t str);
