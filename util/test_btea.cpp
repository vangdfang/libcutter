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
 * Should you need to contact us, the author, you can do so at
 * http://github.com/vangdfang/libcutter
 */

#include <iostream>
#include <stdint.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "btea.h"

using std::cout;
using std::endl;

static const uint32_t keys[4]          = {   12,   34,   56,  78  };
static const uint8_t  known_result[]   = { 0xCE, 0x9D, 0x7D, 0x67, 0x30, 0x53, 0x70, 0x3F, 0x04, 0x58, 0x6A, 0xA1 };

void print_hex( const uint8_t * ptr, int numBytes )
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
    print_hex( (const uint8_t*)buffer, size );

    btea( buffer, 3, keys);

    cout << endl;
    cout << "After encryption:" << endl;
    cout << '\t';
    print_hex( (const uint8_t*)buffer, size );

    if( ptr != args[1] )
    {
        cout << "Secret default " << ptr <<" test mode enabled...(add another string as arg if you like)" << endl;

        if( memcmp( known_result, buffer, sizeof( known_result ) ) == 0 )
        {
            cout << "\tHuzzah! You passed the default btea test" << endl;
        }
        else
        {
            cout << "\tCrapCrap! You failed the default btea test" << endl;
            cout << "\t";
            print_hex( known_result, sizeof( known_result ) );
            cout << "\t... should've been the result" << endl;
            cout << "\tTry running test_endian to find out why"    << endl;
        }

        btea( buffer, -3, keys );
        cout << "After decrypting, result was:";
        print_hex( (const uint8_t*)buffer, size );

        if( memcmp( buffer, ptr, size ) == 0 )
        {
            cout << "Hurray, you passed the btea encryption identity test" << endl;
        }
        else
        {
            cout << "You failed the btea encryption identity test" << endl;
        }
    }

    free( buffer );
}
