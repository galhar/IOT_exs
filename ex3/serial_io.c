//
// Created by osboxes on 11/16/20.
//

#include "serial_io.h"

int fd;

int SerialInit(char* port, unsigned int baud) {
    struct termios SerialPortSettings;
    if((fd = open("/dev/ttyUSB0",O_RDWR | O_NOCTTY)) == ERROR) {
        perror("Error! can not open serial port");
        return ERROR;
    }
    tcgetattr(fd, &SerialPortSettings);
    cfsetispeed(&SerialPortSettings,B115200);
    cfsetospeed(&SerialPortSettings,B115200);
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
    struct termios SerialPortSettings;
    tcgetattr(fd, &SerialPortSettings);
    SerialPortSettings.c_cflag |= CREAD | CLOCAL;//enable receiver
    SerialPortSettings.c_cc[VTIME] = timeout_ms;  /* Wait indefinitely   */
    tcsetattr(fd,TCSANOW,&SerialPortSettings);
    return read(fd,&buf,max_len);
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
