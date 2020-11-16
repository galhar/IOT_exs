//
// Created by osboxes on 11/16/20.
//

#include "serial_io.h"

int SerialInit(char* port, unsigned int baud) {
    int fd;
    struct termios SerialPortSettings;
    if((fd = open("/dev/ttyUSB0",O_RDWR | O_NOCTTY)) == ERROR) {
        perror("Error! can not open serial port");
        return ERROR;
    }
    tcgetattr(fd, &SerialPortSettings);
    cfsetispeed(&SerialPortSettings,B9600);
    cfsetospeed(&SerialPortSettings,B9600);





}