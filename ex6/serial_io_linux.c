//
// Created by osboxes on 11/16/20.
//

#include <zconf.h>
#include "serial_io.h"

int fd;

int SerialInit(char* port, unsigned int baud) {
    struct termios SerialPortSettings;
    if((fd = open(port,O_RDWR | O_NOCTTY)) == ERROR) {
        perror("Error! can not open serial port");
        return ERROR;
    }
    tcgetattr(fd, &SerialPortSettings);
    cfsetispeed(&SerialPortSettings,baud);
    cfsetospeed(&SerialPortSettings,baud);
    SerialPortSettings.c_cflag &= ~PARENB;   // No Parity
    SerialPortSettings.c_cflag &= ~CSTOPB; //Stop bits = 1
    SerialPortSettings.c_cflag &= ~CSIZE; /* Clears the Mask       */
    SerialPortSettings.c_cflag |=  CS8;   /* Set the data bits = 8 */

    SerialPortSettings.c_cflag &= ~CRTSCTS;
    SerialPortSettings.c_cflag |= CREAD | CLOCAL;
    SerialPortSettings.c_iflag &= ~(IXON | IXOFF | IXANY);
    SerialPortSettings.c_iflag &= ~(ICANON | ECHO | ECHOE | ISIG);

    tcsetattr(fd,TCSANOW,&SerialPortSettings);

    return SUCCESS;
}





int SerialRecv(unsigned char *buf, unsigned int max_len, unsigned int timeout_ms) {
    fd_set set;
    struct termios SerialPortSettings;
    int rv = 1;
    struct timeval timeout;
    timeout.tv_usec =  (timeout_ms % 1000)*1000;
    timeout.tv_sec = timeout_ms / 1000;

    tcgetattr(fd, &SerialPortSettings);
    FD_ZERO(&set); /* clear the set */
    FD_SET(fd, &set); /* add our file descriptor to the set */
    tcsetattr(fd,TCSANOW,&SerialPortSettings);
    //rv = select(fd + 1, &set, NULL, NULL, &timeout);
    if(rv == ERROR) {
        perror("select ERROR\n"); /* an error accured */
        return ERROR;
    }
    else if(rv == SUCCESS) {
        printf("timeout ERROR\n"); /* a timeout occured */
        return TIMEOUT;
    }
    else
        return read(fd,buf,max_len);
}

int SerialSend(unsigned char *buf, unsigned int size) {
    return write(fd,buf,size);
}

void SerialFlushInputBuff(void) {
    tcflush(fd, TCIFLUSH);
}

int SerialDisable(void) {
    close(fd);
}
