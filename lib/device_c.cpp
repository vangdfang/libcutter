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
#include <string>
#include <cstdlib>
#include <iostream>
#include <assert.h>
#include "btea.h"
#include "device_c.hpp"

static inline uint32_t htocl( const uint32_t input );

struct __attribute__( __packed__ ) lmc_command
{
    uint8_t  bytes;
    uint8_t  cmd;
    uint32_t data[3];
};

namespace Device
{
    static const float INCHES_TO_C_UNITS = 404.0f;
    static const float C_UNITS_TO_INCHES = ( 1 / (INCHES_TO_C_UNITS) );

    static const uint8_t cmd_stop[] ={0x04, 0x22, 0x00, 0x00, 0x00 };
    static const uint8_t cmd_start[]={0x04, 0x21, 0x00, 0x00, 0x00 };

    C::C()
        : m_serial()
    {
    }

    C::C( const std::string filename )
        : m_serial()
    {
        init( filename );
    }

    C::~C()
    {
    }

    void C::init( const std::string filename )
    {
        m_serial.p_open( filename );
    }

    bool C::move_to( const xy &pt )
    {
        return do_command( pt, m_move_key );
    }

    bool C::cut_to( const xy &pt )
    {
        return do_command( pt, m_line_key );
    }

    bool C::curve_to( const xy &p0, const xy &p1, const xy &p2, const xy &p3 )
    {
        if( !do_command( p0, m_curve_key ) )
        {
            return false;
        }
        if( !do_command( p1, m_curve_key ) )
        {
            return false;
        }
        if( !do_command( p2, m_curve_key ) )
        {
            return false;
        }
        if( !do_command( p3, m_curve_key ) )
        {
            return false;
        }
        return true;
    }

    bool C::start()
    {
        return m_serial.p_write( cmd_start, sizeof( cmd_start ) );
    }

    bool C::stop()
    {
        return m_serial.p_write( cmd_stop, sizeof( cmd_stop ) );
    }

    xy C::convert_to_internal( const xy &input )
    {
        xy buf;
        buf.x = INCHES_TO_C_UNITS * input.x;
        buf.y = INCHES_TO_C_UNITS * input.y;
        return buf;
    }

    bool C::do_command( const xy &pt, const ckey_type k )
    {
        uint8_t rbuf[5];
        xy ptbuffer = convert_to_internal( pt );
        lmc_command l;

        l.bytes  =13;
        l.cmd    = 0x40;
        l.data[0]=htocl( get_rand() );
        l.data[1]=htocl( ptbuffer.y );
        l.data[2]=htocl( ptbuffer.x );
        btea(l.data, 3, k );

        bool ret = false;
        if( m_serial.p_write( (uint8_t*)&l, sizeof( l ) ) == sizeof( l ) )
        {
            int num_chars = m_serial.p_read( rbuf, sizeof( rbuf ) );
            if ( 5 != num_chars )
            {
                std::cout << "expected 5, got " << num_chars << std::endl;
                assert( false );
            }
            ret = true;
        }
        return ret;
    }

    xy C::get_dimensions()
    {
        xy buf;
        buf.x = 6;
        buf.y = 12;
        return buf;
    }
}

/******************************************
Host endianness TO device C endianness Long

Device C talks little endian, so
iff LE, return LE
iff BE, return LE
else assert
******************************************/
static inline uint32_t htocl( const uint32_t input )
{
#if defined(  _BIG_ENDIAN )
return ( ( input & 0x000000FF ) << 24 ) |
       ( ( input & 0x0000FF00 ) << 8  ) |
       ( ( input & 0x00FF0000 ) >> 8  ) |
       ( ( input & 0xFF000000 ) >> 24 ) ;
#elif defined( _MIDDLE_ENDIAN ) || defined( _PDP_ENDIAN )
return ( ( input & 0x0000FFFF ) << 16 ) |
       ( ( input & 0xFFFF0000 ) >> 16 ) ;
#else
//Assume little endian
return input;
#endif
}
