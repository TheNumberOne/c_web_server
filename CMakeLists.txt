cmake_minimum_required(VERSION 3.13)
project(Project2 C)

find_package(Threads)

set(CMAKE_C_STANDARD 11)
link_libraries(${CMAKE_THREAD_LIBS_INIT})

add_executable(server server.c errorHandling.c errorHandling.h http.c http.h string.h string.c response.c response.h uri.c uri.h httpHeaders.c httpHeaders.h httpRequest.c httpRequest.h channel.c channel.h httpWorkerThread.c httpWorkerThread.h loggerWorkerThread.c loggerWorkerThread.h cache.c cache.h mimeTypes.c mimeTypes.h result.h input.c input.h)
add_executable(client client.c string.c string.h)
