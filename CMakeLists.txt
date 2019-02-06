cmake_minimum_required(VERSION 3.13)
project(Project2 C)

set(CMAKE_C_STANDARD 11)

add_executable(Project2 main.c)
add_executable(server server.c errorHandling.c errorHandling.h http.c http.h string.h string.c response.c response.h uri.c uri.h)
add_executable(client client.c string.c string.h)