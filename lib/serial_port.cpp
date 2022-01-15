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
 * Should you need to contact us, the author, you can do so at
 * http://github.com/vangdfang/libcutter
 */
#include "serial_port.hpp"
#include <cstdio>
#include <iomanip>
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

serial_port::serial_port()
{
    debug = 0;
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

void serial_port::set_debug(int level)
{
    debug = level;
}

void serial_port::p_open( const std::string & filename, int baud_rate_ )
{
    speed_t baud_rate = baud_rate_;
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
        sstruct.custom_divisor = sstruct.baud_base / baud_rate;
        sstruct.flags &= ~ASYNC_SPD_MASK;
        sstruct.flags |= ASYNC_SPD_MASK & ASYNC_SPD_CUST;
        double trueBaud = (double)sstruct.baud_base / (double)sstruct.custom_divisor;

        int r = ioctl( fd, TIOCSSERIAL, &sstruct );
        if(debug)
        {
            std::cerr<<"Serial Port Parameters:"<<std::endl;;
            std::cerr<<"\tTargetBaud=" << baud_rate_ << std::endl;
            std::cerr<<"\tBaudBase=" << sstruct.baud_base << std::endl;
            std::cerr<<"\tDivisor=" << sstruct.custom_divisor << std::endl;
            std::cerr<<"\tTrueBaud=" << trueBaud << std::endl;
            std::cerr<<"\tMisMatch=" << 100.0 * ( 1.0 - trueBaud / baud_rate_ ) << "%" <<std::endl;
            std::cerr<<"\tr=" << r <<std::endl;
        }
        #elif( __APPLE__ )
        if( ioctl( fd, IOSSIOSPEED, &baud_rate ) == -1 )
        {
            std::cerr << "driver may not support IOSSIOSPEED" << std::endl;
        }
        #endif
    }
    else
    {
        std::cerr << "unable to open serial port:" << filename << std::endl;
    }
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
    std::cerr << "port closed" << std::endl;
}


size_t serial_port::p_write( const uint8_t * data, size_t size )
{
    int      count = 0;
    char     prev_fill;

    if(debug)
    {
        std::cerr << "p_write(" << size << "):";
        //config stream
        prev_fill = std::cerr.fill();
	    std::cerr << "0x";
        std::cerr << std::hex;
        std::cerr.fill('0');
    }

    for( size_t i = 0; i < size; ++i )
    {
	delay(1000);

        if( write( fd, data, 1 ) == 1 )
        {
            if(debug) std::cerr << std::setw(2) << (int)*data;
            data++;
            count++;
        }
        else
        {
            break;
        }

    }

    if(debug)
    {
        std::cerr << std::endl;
        //unconfig stream
        std::cerr << std::dec;
        std::cerr.fill(prev_fill);
    }

    return count;
}


size_t serial_port::p_read( uint8_t * data, size_t size )
{
    char prev_fill;

    if(debug)
    {
        std::cerr << "p_read(" << size << "):";
        //config stream
        prev_fill = std::cerr.fill();
	    std::cerr << "0x";
        std::cerr << std::hex;
        std::cerr.fill('0');
    }

    if( fd < 0 )
    {
        std::cerr<<"Error reading from closed port"<<std::endl;
    }
    int retn = read( fd, (void*)data, size );

    if(debug)
    {
        for( ssize_t i = 0; i < retn; ++i )
        {
            std::cerr << std::setw(2) << (int)data[i];
        }
        std::cerr << std::endl;
        //unconfig stream
        std::cerr << std::dec;
        std::cerr.fill(prev_fill);
    }

    return retn < 0 ? 0 : retn;
}


uint64_t serial_port::getTime( void )
{
    timeval tv;
    gettimeofday( &tv, NULL );
    return (uint64_t)tv.tv_sec * 1000000 + (uint64_t)tv.tv_usec ;
}

int serial_port::delay( int usecs )
{
    return usleep(usecs);
}
