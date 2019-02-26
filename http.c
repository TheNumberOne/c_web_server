#include <memory.h>
#include "http.h"
#include "response.h"
#include "uri.h"
#include "httpRequest.h"
#include "mimeTypes.h"
#include "loggerWorkerThread.h"
#include "php.h"


string_t handleTarget(fileCache_t cache, httpRequest_t request);

void handleHttpConnection(int socketFd, channel_t logger, fileCache_t cache) {
    httpRequest_t request;

    // First read response
    httpResponse_t response = readRequest(socketFd, &request);
    if (response != NULL) {
        logMessageWithIpC(logger, socketFd, "Invalid request.");
        writeResponse(socketFd, response);
        close(socketFd);
        destroyHttpResponse(response);
        return;
    }

    // Then read the target
    string_t s = handleTarget(cache, request);

    // if file doesn't exist return proper error
    if (s == NULL) {
        string_t message = stringFromCString("File not found: ");
        string_t file = uriToString(request->target);
        plusEqual(message, file);
        destroyString(file);
        logMessageWithIp(logger, socketFd, message);

        response = createHttpResponse();
        httpResponseStatus(response, HTTP_STATUS_CODE_NOT_FOUND);
        writeResponse(socketFd, response);
        destroyHttpResponse(response);
        destroyHttpRequest(request);
        close(socketFd);
        return;
    }

    // return the file
    logMessageWithIpC(logger, socketFd, "Successful request.");
    response = createHttpResponse();
    httpResponseStatus(response, HTTP_STATUS_CODE_OK);
    setHttpContent(response, s);
    addContentLengthHeader(response);
    addHeader(response, stringFromCString("Content-Type"), getFileContentType(request->target));
    writeResponse(socketFd, response);

    destroyHttpResponse(response);
    destroyHttpRequest(request);
    close(socketFd);
}

string_t handleTarget(fileCache_t cache, httpRequest_t request) {
    char *path = uriToFilePath(request->target);
    if (path == NULL) return NULL;

    char *extension = getFileExtension(request->target);
    if (strcmp(extension, "php") == 0) {
        string_t s = readPhpFile(getFileCacheWorkingDirectory(cache), path);
        free(path);
        return s;
    } else {
        string_t s = fileCacheGetFile(cache, path);
        free(path);
        return s;
    }
}

