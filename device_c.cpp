#include <string>
#include <cstdlib>
#include "btea.h"
#include "device_c.hpp"

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
    {
    }

    C::C( const std::string filename )
    {
        init( filename );
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
        l.data[0]=get_rand();
        l.data[1]=ptbuffer.y;
        l.data[2]=ptbuffer.x;
        btea(l.data, 3, k );

        bool ret = false;
        if( m_serial.p_write( (uint8_t*)&l, sizeof( l ) ) == sizeof( l ) )
        {
            m_serial.p_read( rbuf, sizeof(rbuf ) );
            ret = true;
        }
        return ret;
    }

}
