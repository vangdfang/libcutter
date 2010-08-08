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
 * Should you need to contact us, the author, you can do so either at
 * http://github.com/vangdfang/libcutter, or by paper mail:
 *
 * libcutter Developers @ Cowtown Computer Congress
 * 3101 Mercier Street #404, Kansas City, MO 64111
 */

#include "serial_port.hpp"
#include "test_serial.hpp"

#include <cstdio>
#include <iostream>
#include <vector>
#include <signal.h>
#include <stdlib.h>

using std::printf;
using std::cout;
using std::endl;

int main(int argc, char* argv[])
{
    signal(SIGINT, clean_up);
    ports.push_back(new serial_port("/dev/ttyUSB0"));

    serial_port *p = ports.front();

    printf("timing = %f\n", (float)(TIMING_GOAL-TIMING_CONSTRAINT) );

    static  const uint8_t stop[] ={0x04, 0x22, 0x00, 0x00, 0x00 };
    static  const uint8_t start[]={0x04, 0x21, 0x00, 0x00, 0x00 };

    static  const uint8_t cmd1[] ={ 0x0D, 0x40 , 0x13 , 0x48 , 0x61 , 0x77 , 0x15 , 0xc2 , 0x25 , 0x65 , 0x64 , 0xc7 , 0x70 , 0x64 };
    static  const uint8_t cmd2[] ={ 0x0D, 0x40 , 0x1d , 0x0a , 0x74 , 0xd5 , 0x94 , 0x20 , 0xf0 , 0x83 , 0x8a , 0xc4 , 0x7d , 0xba };
    static  const uint8_t cmd3[] ={ 0x0D, 0x40 , 0xec , 0xc3 , 0xa0 , 0xfb , 0x6f , 0xdd , 0xb7 , 0xfa , 0xaa , 0x47 , 0x70 , 0xf5 };
    static  const uint8_t cmd4[] ={ 0x0D, 0x40 , 0xe6 , 0x39 , 0xa4 , 0x3a , 0x6a , 0xd3 , 0x98 , 0xb7 , 0x22 , 0x97 , 0x71 , 0x15 };
    static  const uint8_t cmd5[] ={ 0x0D, 0x40 , 0xf9 , 0x36 , 0x4a , 0xa5 , 0xe6 , 0x82 , 0x96 , 0x99 , 0xe5 , 0x5e , 0x5e , 0x6b };
    static  const uint8_t cmd6[] ={ 0x0D, 0x40 , 0x05 , 0xc5 , 0xb9 , 0x7b , 0x8c , 0x99 , 0x62 , 0x41 , 0x2a , 0x21 , 0xfe , 0x77 };

    if( p->is_open() )
    {
        printf("Port open\n");

        p->p_write( stop, sizeof( stop ) );
        sleep(1);
        p->p_write( start, sizeof( start ) );
        sleep(1);
        p->p_write( cmd1, sizeof( cmd1 ) );
        sleep(1);
        p->p_write( cmd2, sizeof( cmd2 ) );
        sleep(1);
        p->p_write( cmd3, sizeof( cmd3 ) );
        sleep(1);
        p->p_write( cmd4, sizeof( cmd4 ) );
        sleep(1);
        p->p_write( cmd5, sizeof( cmd5 ) );
        sleep(1);
        p->p_write( cmd6, sizeof( cmd6 ) );
        p->p_write( stop, sizeof( stop ) );
    }
    else
    {
        printf("Port not open\n");
    }
    clean_up(0);
}


void clean_up(int signal)
{
    std::vector<serial_port*>::iterator i = ports.begin();
    while( i != ports.end() )
    {
        delete *i;
        i++;
    }
    exit(signal);
}
