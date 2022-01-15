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
#ifndef SERIAL_PORT_HPP
#define SERIAL_PORT_HPP
#include <cstring>
#include <stdint.h>

#if( !__WIN32 )
#include <termios.h>
#if( __linux )
#include <linux/serial.h>
#endif
#else
/* place any win32 includes here */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <string>

#define TIMING_GOAL (.001)
#define TIMING_CONSTRAINT (.00025)
#define BAUD_RATE B38400

class serial_port
{
    public:
        serial_port();
        ~serial_port();

        bool is_open();
        void p_open( const std::string & filename, int baud_rate );
        void p_close();

        std::size_t p_write( const uint8_t * data, std::size_t size );
        std::size_t p_read(  uint8_t * data, std::size_t size );
        int delay(int);
        void set_debug(int);

    protected:
        int debug;
    #if( !__WIN32 )
        int  fd;

        termios       oldtio;
    #if( __linux )
        serial_struct oldsstruct;
    #endif
    #else
        HANDLE fd;
        DCB olddcb;
    #endif

        uint64_t getTime();
};
#endif
