#include "serial_port.hpp"
#include <cstdio>
#include <sys/ioctl.h>
#include <termios.h>
#include <sys/types.h>
#include <cstdlib>
#include <fcntl.h>
#include <sys/time.h>
#include <unistd.h>
#include <cmath>
#include <string>
using std::size_t;

#include <iostream>
using namespace std;

serial_port::serial_port()
{
    fd = -1;
}


serial_port::~serial_port()
{
    p_close();
}


bool serial_port::is_open()
{
    return fd >= 0;
}


serial_port::serial_port( const string & filename )
{
    p_open( filename );
}


void serial_port::p_open( const string & filename )
{
    termios newtio;
    serial_struct sstruct;

    fd = open( filename.c_str(), O_RDWR | O_NOCTTY );
    if( fd >= 0 )
    {
        tcgetattr( fd, &oldtio );
        memset( &newtio, 0x00, sizeof( newtio ) );
        newtio.c_cflag &= ~( PARENB | CSTOPB | CSIZE );
        newtio.c_cflag |= BAUD_RATE | CS8 | CLOCAL | CREAD | CSTOPB;

        newtio.c_iflag &= ~( IXON | IXOFF | INLCR | IGNCR | ICRNL | IUCLC | IMAXBEL | PARMRK );
        newtio.c_iflag |= IGNPAR | IGNBRK | ISTRIP | INPCK ;

        newtio.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG );
        newtio.c_oflag &= ~OPOST;
        newtio.c_cc[VMIN]  = 5;
        newtio.c_cc[VTIME] = 0;
        //255;

        tcflush(fd, TCIFLUSH);
        tcsetattr(fd,TCSANOW,&newtio);

        //ASYNC_SPD_MASK
        ioctl( fd, TIOCGSERIAL, &oldsstruct );
        sstruct = oldsstruct;
        sstruct.custom_divisor = sstruct.baud_base / 200000;
        sstruct.flags &= ~ASYNC_SPD_MASK;
        sstruct.flags |= ASYNC_SPD_MASK & ASYNC_SPD_CUST;
        printf("Divisor=%i\n", sstruct.custom_divisor );

        int r = ioctl( fd, TIOCSSERIAL, &sstruct );
        printf("r=%i\n",r);
    }

    #ifdef SERIAL_PORT_DEBUG_MODE
    #if SERIAL_PORT_DEBUG_MODE
    char cmd[100];
    sprintf(cmd, "stty -F %s", filename.c_str() );
    system(cmd);
    sprintf(cmd, "setserial -a %s", filename.c_str() );
    system(cmd);
    #endif
    #endif
}


void serial_port::p_close()
{
    if( fd >= 0 )
    {
        tcsetattr( fd, TCSANOW, &oldtio );
        ioctl( fd, TIOCSSERIAL, &oldsstruct );
        close( fd );
        fd = -1;
    }
}


size_t serial_port::p_write( const uint8_t * data, size_t size )
{
    int    i;
    int    count = 0;
    uint64_t t1 = getTime();
    for( i = 0; i < size; ++i )
    {
        usleep(100);

        if( write( fd, data, 1 ) == 1 )
        {
            data++;
            count++;
        }
        else
        {
            break;
        }

    }
    std::cout << getTime() - t1 << std::endl;
    return count;
}


size_t serial_port::p_read(  const uint8_t * data, size_t size )
{
    if( fd < 0 )
    {
        cout<<"Error reading from closed port"<<endl;
    }
    return read( fd, (void*)data, size );
}


const uint64_t serial_port::getTime( void )
{
    timeval tv;
    gettimeofday( &tv, NULL );
    return (uint64_t)tv.tv_sec * 1000000 + (uint64_t)tv.tv_usec ;
}
