/*
 * test_btea - Quick encryption test program
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

#include <iostream>
#include <stdint.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "btea.h"
#include "keys.h"

using std::cout;
using std::endl;

static const uint32_t keys[4]={MOVE_KEY_0, MOVE_KEY_1, MOVE_KEY_2, MOVE_KEY_3 };

void print_hex( uint8_t * ptr, int numBytes )
{
    for( int i = 0; i < numBytes; ++i )
    {
        printf( "%02x", *ptr );
        ptr++;
    }
    printf("\n");
}


int main( int numArgs, char *args[] )
{
    const char * ptr;
    uint32_t * buffer;
    int        size;

    if( numArgs != 2 )
    {
        ptr = "TestPhrase!";
    }
    else
    {
        ptr = args[1];
    }

    size = ( ( ( strlen( ptr ) + 3 ) >> 2 ) << 2 );

    cout << "using \'" << ptr << "\' as cleartext" << endl;

    cout << "using a " << size << " byte buffer" << endl;
    buffer = (uint32_t*)malloc( size );

    memset( buffer, 0x00, size );
    memcpy( buffer, ptr, strlen( ptr ) );

    cout << "Before encryption:" << endl;
    print_hex( (uint8_t*)buffer, size );

    btea( buffer, 3, keys);

    cout << endl;
    cout << "After encryption:" << endl;
    print_hex( (uint8_t*)buffer, size );

    free( buffer );
}
