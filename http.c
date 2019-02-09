#include "http.h"
#include "response.h"
#include "uri.h"
#include "httpRequest.h"
#include "mimeTypes.h"
#include "loggerWorkerThread.h"


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
    char *path = uriToFilePath(request->target);
    string_t s = path == NULL ? NULL : fileCacheGetFile(cache, path);
    if (path != NULL) free(path);

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

