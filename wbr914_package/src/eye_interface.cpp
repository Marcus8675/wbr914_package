#include "eye_interface.h"
#include <sys/file.h>


int eye_interface::open_eye()
{
    // Create new termios struct, we call it 'tty' for convention
    // No need for "= {0}" at the end as we'll immediately write the existing
    // config to this struct
    struct termios tty;

    fflush(stdout);
    this->eye_serial_port = open(eye_port, O_RDWR | O_NOCTTY);
    // Check for errors
    if (this->eye_serial_port < 0) 
    {
        printf("Error %i from open: %s\n", errno, strerror(errno));
        return -1;
    }

    // Read in existing settings, and handle any error
    // NOTE: This is important! POSIX states that the struct passed to tcsetattr()
    // must have been initialized with a call to tcgetattr() overwise behaviour
    // is undefined
    if(tcgetattr(this->eye_serial_port, &tty) != 0) 
    {
        printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
        return -1;
    }

    cfsetispeed( &tty, B115200 ); // Set communication speed(baud)
    cfsetospeed( &tty, B115200 );

    // Set timeout to .1 sec
    tty.c_cc[ VTIME ] = 1;
    tty.c_cc[ VMIN ]  = 0;
    tty.c_iflag |= IGNPAR;

    // Save tty settings, also checking for error
    if(tcsetattr(this->eye_serial_port, TCSANOW, &tty) != 0) 
    {
        printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
        close(this->eye_serial_port);
        this->eye_serial_port = -1;
        return -1;
    }
    //printf("The eye is open on port:");
    //printf(eye_port);
    //printf("\n");
    return 1;
};

int eye_interface::set_eye(__uint8_t red, __uint8_t green, __uint8_t blue, __uint8_t white )
{
    // <SETEYE RRR GGG BBB WWW>
    // Write to serial port
    char msg [25];
    open_eye();
    sprintf(msg, "<SETEYE %03d %03d %03d %03d>", red, green, blue, white);
    if (write(this->eye_serial_port, msg, sizeof(msg)) == -1)
    {
        return -1;
        printf("Error writting to eye\n");
    }
    close(this->eye_serial_port);
    return 1;
};

int eye_interface::set_led(__uint8_t led,__uint8_t red, __uint8_t green, __uint8_t blue, __uint8_t white )
{
    // <SETLED # ### ### ### ###>      LED# R G B W
    char msg [27];
    open_eye();
    //printf("eye port handle:%d\n",this->eye_serial_port);
    sprintf(msg, "<SETLED %1d %03d %03d %03d %03d>", led, red, green, blue, white);
    if (write(this->eye_serial_port, msg, sizeof(msg)) != sizeof(msg))
    {
        return -1;
        printf("Error writting to eye.\n");
    }
    close(this->eye_serial_port);
    return 1;
    
};