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
#ifndef DEVICE_CV_SIM_HPP
#define DEVICE_CV_SIM_HPP

#include <stdint.h>
#include <SDL2/SDL.h>

#include "device.hpp"
#include "types.h"

namespace Device
{
    class CV_sim : public Device::Generic
    {
        public:
            CV_sim();
            CV_sim( const std::string filename );
            /* virtual */ bool move_to(const xy &aPoint);
            /* virtual */ bool cut_to(const xy &aPoint);
            /* virtual */ bool curve_to(const xy &p0, const xy &p1, const xy &p2, const xy &p3);
            /* virtual */ bool start();
            /* virtual */ bool stop();
            /* virtual */ xy   get_dimensions();
            SDL_Surface * get_image();
            bool set_tool_width( const float tool_width );

        private:
            xy convert_to_internal( const xy &input );
            xy convert_to_external( const xy &input );
            xy current_position;
            std::string output_filename;
            bool running;
            SDL_Window * screen;
            SDL_Renderer * renderer;
            float tool_width;
    };
}
#endif
