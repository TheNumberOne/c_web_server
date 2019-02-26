
#include <unistd.h>
#include "php.h"

string_t readPhpFile(int workingDirectory, const char *path) {
    int ends[2];
    if (pipe(ends) == -1) return NULL;

    switch (fork()) {
        case -1:
            close(ends[0]);
            close(ends[1]);
            return NULL;
        case 0:
            // for debugging, execute before fork in gdb:
            // set follow-fork-mode child
            // set detach-on-fork off
            dup2(ends[1], STDOUT_FILENO);
            close(ends[0]);
            close(ends[1]);
            fchdir(workingDirectory);
            const char *args[] = {"/usr/bin/php", "-f", path, NULL};
            execv("/usr/bin/php", (char *const *) args);

            //Code should never get to here
            return NULL;
        default:
            close(ends[1]);
            string_t output = createString();

            char c;
            ssize_t readResult;
            while ((readResult = read(ends[0], &c, 1)) > 0) {
                append(output, c);
            }
            close(ends[0]);
            if (readResult == -1) {
                destroyString(output);
                return NULL;
            }
            return output;
    }
}
