cmake_minimum_required(VERSION 3.13)
project(ex5)

set(CMAKE_CXX_STANDARD 14)
add_executable(ex5 main.c HTTP_client.c HTTP_client.h socket_linux_modem.c socket.h cellular.c cellular.h serial_io_linux.c serial_io.h)