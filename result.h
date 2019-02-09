#pragma once

enum result {
    OK,
    IO_ERROR,
    OOM_ERROR,
    EOF_ERROR,
    INVALID_METHOD,
    INVALID_VERSION,
    INVALID_HEADER
};
typedef enum result result_t;