/*
 * libcutter - xy cutter control library
 * Copyright (c) 2010 - libcutter Developers
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Should you need to contact us, the author, you can do so either at
 * http://github.com/vangdfang/libcutter, or by paper mail:
 *
 * libcutter Developers @ Cowtown Computer Congress
 * 3101 Mercier Street #404, Kansas City, MO 64111
 */
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
#if( __APPLE__ )
#include <IOKit/serial/ioss.h>
#endif

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

    fd = open( filename.c_str(), O_RDWR | O_NOCTTY );
    if( fd >= 0 )
    {
        tcgetattr( fd, &oldtio );
        memset( &newtio, 0x00, sizeof( newtio ) );
        newtio.c_cflag &= ~( PARENB | CSIZE );
        newtio.c_cflag |= BAUD_RATE | CS8 | CLOCAL | CREAD | CSTOPB;

        newtio.c_iflag &= ~( IXON | IXOFF | INLCR | IGNCR | ICRNL | IMAXBEL | PARMRK );
        newtio.c_iflag |= IGNPAR | IGNBRK | ISTRIP | INPCK ;

        newtio.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG );
        newtio.c_oflag &= ~OPOST;
        newtio.c_cc[VMIN]  = 5;
        newtio.c_cc[VTIME] = 0;
        //255;

        tcflush(fd, TCIFLUSH);
        tcsetattr(fd,TCSANOW,&newtio);

        #if( __linux )
        //ASYNC_SPD_MASK
        serial_struct sstruct;
        ioctl( fd, TIOCGSERIAL, &oldsstruct );
        sstruct = oldsstruct;
        sstruct.custom_divisor = sstruct.baud_base / 200000;
        sstruct.flags &= ~ASYNC_SPD_MASK;
        sstruct.flags |= ASYNC_SPD_MASK & ASYNC_SPD_CUST;
        printf("Divisor=%i\n", sstruct.custom_divisor );

        int r = ioctl( fd, TIOCSSERIAL, &sstruct );
        printf("r=%i\n",r);
        #elif( __APPLE__ )
        speed_t baud_rate = 200000;
        if( ioctl( fd, IOSSIOSPEED, &baud_rate ) == -1 )
        {
            std::cout << "driver may not support IOSSIOSPEED" << std::endl;
        }
        #endif
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
        #if( __linux )
        ioctl( fd, TIOCSSERIAL, &oldsstruct );
        #endif
        close( fd );
        fd = -1;
    }
    std::cout << "port closed" << std::endl;
}


size_t serial_port::p_write( const uint8_t * data, size_t size )
{
    size_t   i;
    int      count = 0;
    for( i = 0; i < size; ++i )
    {
        usleep(1000);

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
    return count;
}


size_t serial_port::p_read( uint8_t * data, size_t size )
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

int serial_port::delay( int usecs )
{
    return usleep(usecs);
}
