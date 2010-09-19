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
#ifndef DEVICE_C_HPP
#define DEVICE_C_HPP

#include <stdint.h>
#include <cstring>
#include "device.hpp"
#include "types.h"
#include "serial_port.hpp"

namespace Device
{
    class C : public Device::Generic
    {
        public:
            C();
            C( const std::string filename );
            ~C();
            /* virtual */ void init( const std::string filename );
            /* virtual */ bool move_to( const xy &aPoint );
            /* virtual */ bool cut_to( const xy &aPoint );
            /* virtual */ bool curve_to( const xy &p0, const xy &p1, const xy &p2, const xy &p3 );
            /* virtual */ bool start();
            /* virtual */ bool stop();
            /* virtual */ xy   get_dimensions();
            inline void set_move_key( ckey_type k )
            {
                memcpy( m_move_key, k, sizeof(ckey_type) );
            }
            inline void set_line_key( ckey_type k )
            {
                memcpy( m_line_key, k, sizeof(ckey_type) );
            }
            inline void set_curve_key( ckey_type k )
            {
                memcpy( m_curve_key, k, sizeof(ckey_type) );
            }
            inline bool is_open()
            {
                return m_serial.is_open();
            }
        private:
            inline int get_rand() const { return 12345; };
            xy convert_to_internal( const xy &input );
            bool do_command( const xy &pt, const ckey_type k );
            ckey_type m_move_key;
            ckey_type m_line_key;
            ckey_type m_curve_key;
            serial_port m_serial;
    };
}
#endif
