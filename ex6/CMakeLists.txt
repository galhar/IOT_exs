cmake_minimum_required(VERSION 3.13)
project(ex2)

set(CMAKE_CXX_STANDARD 14)
add_executable(ex6 main.c MQTTClient.c MQTTClient.h wolfmqtt/mqtt_client.c wolfmqtt/mqtt_packet.c wolfmqtt/mqtt_socket.c
        wolfmqtt/mqtt_client.h wolfmqtt/mqtt_packet.h wolfmqtt/mqtt_socket.h wolfmqtt/options.h socket_linux_modem.c socket.h
        cellular.c cellular.h serial_io.h serial_io_linux.c)