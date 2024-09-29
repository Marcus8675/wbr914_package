// C library headers
#include <stdio.h>
#include <string.h>

// Linux headers
#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()

#define eye_port "/dev/serial/by-id/usb-1a86_USB_Serial-if00-port0"

#ifndef EYEINTERFACE_H
#define EYEINTERFACE_H

class eye_interface
{
    public:
    int eye_serial_port;

    int open_eye();
    int set_eye(__uint8_t red, __uint8_t green, __uint8_t blue, __uint8_t white );
    int set_led(__uint8_t led,__uint8_t red, __uint8_t green, __uint8_t blue, __uint8_t white );
    private:

};

#endif