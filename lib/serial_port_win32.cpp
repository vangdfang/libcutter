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
#include <sys/types.h>
#include <cstdlib>
#include <sys/time.h>
#include <unistd.h>
#include <cmath>
#include <string>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

using std::size_t;

#include <iostream>
using namespace std;

serial_port::serial_port()
{
    debug = 0;
    fd = INVALID_HANDLE_VALUE;
}


serial_port::~serial_port()
{
    p_close();
}


bool serial_port::is_open()
{
    return fd != INVALID_HANDLE_VALUE;
}

void serial_port::set_debug(int level)
{
    debug = level;
}

void serial_port::p_open( const string & filename, int baud_rate )
{
    DCB newdcb = { 0 };
    COMMTIMEOUTS newtimeouts = { 0 };

    fd = CreateFile( filename.c_str(), GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0 );
    if( fd != INVALID_HANDLE_VALUE )
    {
        if( !GetCommState( fd, &olddcb ) )
        {
            // Could not save comm state
        }
        newdcb.DCBlength = sizeof( newdcb );
        newdcb.BaudRate = baud_rate;
        newdcb.ByteSize = 8;
        newdcb.StopBits = ONESTOPBIT;
        newdcb.Parity = NOPARITY;

        if( !SetCommState( fd, &newdcb ) )
        {
            // Could not set comm state
        }
        newtimeouts.ReadIntervalTimeout = 500;
        newtimeouts.ReadTotalTimeoutConstant = 0;
        newtimeouts.ReadTotalTimeoutMultiplier = 0;
        newtimeouts.WriteTotalTimeoutConstant = 50;
        newtimeouts.WriteTotalTimeoutMultiplier = 10;
        if( !SetCommTimeouts( fd, &newtimeouts ) )
        {
            // Could not set timeouts
        }
    }
}


void serial_port::p_close()
{
    if( fd != INVALID_HANDLE_VALUE )
    {
        CloseHandle( fd );
        fd = INVALID_HANDLE_VALUE;
    }
    std::cout << "port closed" << std::endl;
}


size_t serial_port::p_write( const uint8_t * data, size_t size )
{
    size_t   i;
    int      count = 0;
    DWORD    bytesWritten = 0;

    if( fd == INVALID_HANDLE_VALUE )
    {
        return 0;
    }

    for( i = 0; i < size; ++i )
    {
	delay(1000);

        if( WriteFile( fd, data, 1, &bytesWritten, NULL ) )
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
    DWORD bytesRead = 0;

    if( fd == INVALID_HANDLE_VALUE )
    {
        std::cerr << "Error reading from closed port" << std::endl;
    }

    if( !ReadFile( fd, data, size, &bytesRead, NULL ) )
    {
        return 0;
    }

    return bytesRead;
}


uint64_t serial_port::getTime( void )
{
    timeval tv;
    gettimeofday( &tv, NULL );
    return (uint64_t)tv.tv_sec * 1000000 + (uint64_t)tv.tv_usec ;
}


int serial_port::delay( int usecs )
{
    Sleep((usecs+999)/1000);
    return 0;
}
