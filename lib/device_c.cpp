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
#include <string>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <assert.h>
#include "btea.h"
#include "device_c.hpp"

static inline uint32_t htocl( const uint32_t input );

struct __attribute__(( packed )) lmc_command
{
    uint8_t  bytes;
    uint8_t  cmd;
    uint32_t data[3];
};

namespace Device
{
    static const int DELAY = 170000;
    static const float INCHES_TO_C_UNITS = 404.0f;
    static const float C_UNITS_TO_INCHES = ( 1 / (INCHES_TO_C_UNITS) );

    C::C()
        : m_serial()
    {
        m_version_major = -1;
        m_version_minor = -1;
        m_model_id = model_id::UNKNOWN;
    }

    C::C( const std::string filename )
        : m_serial()
    {
        m_version_major = -1;
        m_version_minor = -1;
        m_model_id = model_id::UNKNOWN;
        init( filename );
    }

    C::~C()
    {
    }

    void C::init( const std::string filename )
    {
        m_serial.p_open( filename, baud_rate );
        enumerate();
        if(model_id::UNKNOWN) enumerate();//Try twice.
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
        m_serial.delay(DELAY);
        return do_command_32( 0x21 );
    }

    bool C::stop()
    {
        m_serial.delay(DELAY);
        return do_command_32( 0x22 );
    }

    bool C::do_command_32( uint32_t command )
    {
        uint8_t tbuf[5] = { 4 };
        uint32_t command_in_c_endian = htocl( command );
        memcpy( tbuf + 1, &command_in_c_endian, sizeof command_in_c_endian );

        if( m_serial.p_write( tbuf, sizeof(tbuf) ) != sizeof(tbuf ) )
        {
            std::cerr <<"failed to send command " << command <<"(" << sizeof(tbuf)<< " bytes)"<< std::endl;
            return false;
        }
        return true;
    }

    bool C::enumerate()
    {
        if( !do_command_32( 0x12 ) )
        {
            return false;
        }

        std::vector<uint8_t> rbuf = read_response();

        //parse out response
        int n_bytes = rbuf[0];
        int unk_zero_1 = rbuf[1];
        int model_id = rbuf[2];
        int unk_zero_3 = rbuf[3];
        int version_major = rbuf[4];
        int unk_zero_5 = rbuf[5];
        int version_minor = rbuf[6];

        if( n_bytes != 6 )
        {
            std::cout <<"version length response mismatch: " << n_bytes << " bytes" << std::endl;
            return false;
        }

        if( unk_zero_1 || unk_zero_3 || unk_zero_5 )
        {
            std::cout <<"unknown non-zero byte. Please report model and version number to project" << std::endl;
            return false;
        }

        m_version_major = version_major;
        m_version_minor = version_minor;

        switch( model_id )
        {
            case 0x14:
                m_model_id = model_id::EXPRESSION;
                break;

            case 0x1e:
                m_model_id = model_id::MINI;
                break;

            case 0x0a:
                m_model_id = model_id::PERSONAL;
                break;

            default:
                std::cout <<"unknown model identifier(0x" << std::hex << model_id;
                std::cout << std::dec <<"). Please report model, model id, and firmware version to project" << std::endl;
                return false;
        }
        return true;
    }

    std::string C::device_make()
    {
        return "Cricut";
    }

    std::string C::device_model()
    {
        switch( m_model_id )
        {
            case model_id::EXPRESSION:
                return "Expression";

            case model_id::MINI:
                return "Mini";

            case model_id::PERSONAL:
                return "Personal";

            default:
                return "Unknown";
        }
    }

    std::string C::device_version()
    {
        if( m_version_major >= 0 && m_version_minor >= 0 )
        {
            return std::to_string(m_version_major) + "." + std::to_string(m_version_minor);
        }
        return "UnknownVersion";
    }

    int C::get_version_major()
    {
        return m_version_major;
    }

    int C::get_version_minor()
    {
        return m_version_minor;
    }

    xy C::convert_to_internal( const xy &input )
    {
        return xy( INCHES_TO_C_UNITS * input.x, INCHES_TO_C_UNITS * input.y );
    }

    std::vector<uint8_t> C::read_response( void )
    {
        std::vector<uint8_t> retn;

        uint8_t sz;
        if( m_serial.p_read( &sz, 1 ) != 1 )
        {
            return retn;
        }

        retn.push_back( sz );

        retn.resize( 1 + sz, 0 );
        int i_read = m_serial.p_read( &retn[1], sz );
        if( i_read != sz )
        {
            retn.resize( 1 + i_read );
        }

        return retn;
    }

    bool C::do_command( const xy &pt, const ckey_type k )
    {
        xy ptbuffer = convert_to_internal( pt );
        lmc_command l;

        l.bytes  =13;
        l.cmd    = 0x40;
        l.data[0]=htocl( get_rand() );
        l.data[1]=htocl( ptbuffer.y );
        l.data[2]=htocl( ptbuffer.x );
        btea(l.data, 3, k );

        if( m_serial.p_write( (uint8_t*)&l, sizeof( l ) ) != sizeof( l ) )
        {
            std::cerr <<"failed to do command("<<sizeof(l)<<" bytes)" << std::endl;
            return false;
        }

        std::vector<uint8_t> rbuf = read_response();

        if( rbuf.size() != 5 )
        {
            std::cerr << "command response packet: expected 5 bytes, got " << rbuf.size() << std::endl;
            return false;
        }

        if( rbuf[0] != 4 )
        {
            std::cerr << "command response data: expected 4, got " << rbuf[0] << std::endl;
            return false;
        }

        return true;
    }

    xy C::get_dimensions()
    {
        switch( m_model_id )
        {
            case model_id::EXPRESSION:
                return xy(12,12);

            case model_id::MINI:
                return xy(8.5,12);;

            case model_id::PERSONAL:
            default://Legacy code
                return xy(6,12);
        }
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
