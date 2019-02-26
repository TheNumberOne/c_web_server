#pragma once

/**
 * Returns a pointer to the first character of the file extension in the uri.
 * Note that the uri owns this location.
 */
char *getFileExtension(uri_t uri);

/**
 * Returns the mime_type for the file at the specified url.
 * @param pUri Does not take ownership
 * @return The mime type. Ownership passed to caller.
 */
string_t getFileContentType(uri_t pUri);