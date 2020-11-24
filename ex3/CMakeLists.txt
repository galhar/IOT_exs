cmake_minimum_required(VERSION 3.13)
project(ex3)

set(CMAKE_CXX_STANDARD 14)
add_executable(ex3 main.c serial_io.c serial_io.h ../ex4/cellular.h ../ex4/cellular.c ../ex4/serial_io_linux.c)