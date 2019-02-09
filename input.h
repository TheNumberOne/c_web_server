#pragma once


#include "string.h"
#include "result.h"

/**
 * Reads from the specified file descriptor until it reaches the specified characters.
 * Excludes the specified characters but does remove them from input.
 * @param fd the file to read from. not closed.
 * @param pString The location to store what was read
 * @param chars The chars to read until. Ownership not taken
 */
result_t readUntilChars(int fd, string_t *pString, const char *chars);

/**
 * Reads from the specified file descriptor until it reaches the specified character.
 * @param fd the file to read from. not closed.
 * @param pString the location to store what was read.
 * @param c The character to read until
 */
result_t readUntil(int fd, string_t *pString, char c);