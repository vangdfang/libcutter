/*
 * test_endian - Quick encryption test program
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

using std::cout;
using std::endl;

void print_hex( const uint8_t * ptr, int numBytes )
{
    for( int i = 0; i < numBytes; ++i )
    {
        printf( "%02x", *ptr );
        ptr++;
    }
    printf("\n");
}


static const uint8_t  LITTLE_ENDIAN_WORD[4] = { 'X', 'I', 'N', 'U' };
static const uint8_t     BIG_ENDIAN_WORD[4] = { 'U', 'N', 'I', 'X' };
static const uint8_t   PDP11_ENDIAN_WORD[4] = { 'N', 'U', 'X', 'I' };
                                 // 0xUNIX
static const uint32_t NATIVE_ENDIAN_WORD[1] =
{
    0x554E4958
};

int main( int numArgs, char *args[] )
{
    bool failed = false;

    if( sizeof( LITTLE_ENDIAN_WORD ) != sizeof( NATIVE_ENDIAN_WORD )  )
    {
        cout << "Failed byte-level packing test" << endl;
        failed = true;
    }

    if( memcmp( NATIVE_ENDIAN_WORD, BIG_ENDIAN_WORD, sizeof( NATIVE_ENDIAN_WORD ) ) == 0 )
    {
        cout << "This appears to be a big-endian machine" << endl;
    }
    else if( memcmp( NATIVE_ENDIAN_WORD, LITTLE_ENDIAN_WORD, sizeof( NATIVE_ENDIAN_WORD ) ) == 0 )
    {
        cout << "This appears to be a little-endian machine" << endl;
    }
    else
    {
        cout << "Failed endian testing" << endl;
        cout << "You'll need to update the btea algorithm" << endl;
        cout << "And then update this testfile" << endl;
        cout << "Please contact the devs with:";

        print_hex( (const uint8_t*)NATIVE_ENDIAN_WORD, sizeof( NATIVE_ENDIAN_WORD ) );

        if( memcmp( NATIVE_ENDIAN_WORD, PDP11_ENDIAN_WORD, sizeof( NATIVE_ENDIAN_WORD ) ) == 0 )
        {
            cout << "Are you on a PDP-11? For real? Sorry, this won't work well at all." << endl;
            cout << "If you contact us, the libcutter developers, C1 would love to help you out" << endl;
            failed = true;
        }
        else
        {
            cout << "I don't know this endian" << endl;
        }
    }

    if( failed )
    {
        cout << "This machine failed 1 or more tests. It is very likely that btea will not perform as expected" << endl;
    }
    else
    {
        cout << "No errors detected" << endl;
    }

    return failed;
}
