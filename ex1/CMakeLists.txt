cmake_minimum_required(VERSION 3.13)
project(ex1)

set(CMAKE_CXX_STANDARD 14)
add_executable(ex1 main.c HTTP_client.c HTTP_client.h socket_linux.c socket.h)