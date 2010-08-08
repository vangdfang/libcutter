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
#ifndef SERIAL_PORT_HPP
#define SERIAL_PORT_HPP
#include <cstring>
#include <stdint.h>
#include <termios.h>
#include <linux/serial.h>

#include <string>

#define TIMING_GOAL (.001)
#define TIMING_CONSTRAINT (.00025)
#define BAUD_RATE B38400

class serial_port
{
    public:
        serial_port();
        serial_port( const std::string & filename );
        ~serial_port();

        bool is_open();
        void p_open( const std::string & filename );
        void p_close();

        std::size_t p_write( const uint8_t * data, std::size_t size );
        std::size_t p_read(  const uint8_t * data, std::size_t size );

    protected:
        int  fd;
        bool stat;

        termios       oldtio;
        serial_struct oldsstruct;

        const uint64_t getTime();
};
#endif
