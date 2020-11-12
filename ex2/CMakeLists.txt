cmake_minimum_required(VERSION 3.13)
project(ex2)

set(CMAKE_CXX_STANDARD 14)
add_executable(ex2 main.c MQTTClient.c MQTTClient.h)