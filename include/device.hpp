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
#ifndef DEVICE_HPP
#define DEVICE_HPP

#include "types.h"
#include <string>

typedef unsigned long _DWORD;

namespace Device
{
    class Generic
    {
        public:
            Generic();
            Generic( std::string aSerial );
            virtual ~Generic();
            virtual inline void init( std::string aSerial ) {};
            virtual inline const std::string device_name() { return "Virtual Device"; };
            virtual bool move_to(const xy &aPoint) = 0;
            virtual bool cut_to(const xy &aPoint) = 0;
            virtual bool curve_to(const xy &p0, const xy &p1, const xy &p2, const xy &p3) = 0;
            virtual bool start() = 0;
            virtual bool stop() = 0;
            inline bool is_connected() { return false; }

            virtual xy get_dimensions() = 0;
    };
}
#endif
