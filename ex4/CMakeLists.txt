cmake_minimum_required(VERSION 3.13)
project(ex4)

set(CMAKE_CXX_STANDARD 14)
add_executable(ex4 main.c serial_io_linux.c serial_io.h cellular.c cellular.h)