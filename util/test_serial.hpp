/*
 * test_serial - Quick test communications program
 * Copyright (c) 2010 - libcutter Developers
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * Should you need to contact us, the author, you can do so at
 * http://github.com/vangdfang/libcutter
 */

#ifndef TEST_SERIAL_HPP
#define TEST_SERIAL_HPP

#include "serial_port.hpp"
#include <vector>

#if( __WIN32 )
#define sleep(x) Sleep(x*1000)
#endif

int main(int argc, char* argv[]);

void clean_up(int signal);

static std::vector<serial_port*> ports;
#endif
