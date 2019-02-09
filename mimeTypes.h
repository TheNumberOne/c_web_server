#pragma once

/**
 * Returns the mime_type for the file at the specified url.
 * @param pUri Does not take ownership
 * @return The mime type. Ownership passed to caller.
 */
string_t getFileContentType(uri_t pUri);