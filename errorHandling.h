//
// Created by thenumberone on 1/29/19.
//

#ifndef PROJECT2_ERRORHANDLING_H
#define PROJECT2_ERRORHANDLING_H

struct result {
    bool ok;
    char *error;
    bool free;
};


void cleanupError(struct result err);

void exitIfError(struct result result);

struct result okResult();

struct result errorFromConstString(const char *e);

struct result errorFromErrno();


#endif //PROJECT2_ERRORHANDLING_H
