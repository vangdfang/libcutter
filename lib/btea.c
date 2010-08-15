/***************************************************
 * This code based on code from 'correction to xtea'
 * David Wheeler djw3@cl.cam.ac.uk
 * Roger Needham rmn@cl.cam.ac.uk
 *
 * www.cix.co.uk/~klockstone/xxtea.pdf
 *
 * this code is in the public domain
 **************************************************/
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include "btea.h"

int btea( uint32_t * v, int32_t n, const uint32_t * k_in )
{
    uint32_t z;
    uint32_t y;
    uint32_t sum;
    uint32_t e;
    uint32_t k[4];
    int status;

    memcpy( &k, k_in, sizeof(uint32_t)*4 );

    //( sqrt( 5 ) - 1 ) / 2
    const uint32_t DELTA=0x9e3779b9 ;

    int32_t p;
    int32_t q;

    //Preinit check
    if( n == 0 )
    {
        assert( 0 );
        return 1;
    }

    #if( _BIG_ENDIAN )
    swap_bytes(v, n);
    #endif

    //Init variables
    z   = v[n-1];
    y   = v[0];
    sum = 0;

    //The core of the algorithm
    #define MX() ( ( ( z >> 5 ) ^ ( y << 2 )) + ( ( y >> 3 ) ^ ( z << 4 ) ) ) ^ ( ( sum ^ y ) + ( k[ ( p & 3 ) ^ e ] ^ z ) )

    if ( n > 1 )
    {
        /* Coding Part */
        q = 6+52/n ;
        while ( q-- > 0 )
        {
            sum += DELTA ;
            e = sum >> 2&3 ;
            for ( p = 0 ; p < n-1 ; p++ )
            {
                y = v[p+1];
                z = v[p] += MX();
            }
            y = v[0];
            z = v[n-1] += MX();
        }
        status = 0;
    }
    else if ( n <-1 )
    {
        /* Decoding Part */
        n = -n ;
        q = 6 + 52 / n ;
        sum = q * DELTA ;
        while (sum != 0)
        {
            e = sum>>2 & 3 ;
            for (p = n-1 ; p > 0 ; p-- )
            {
                z = v[p-1];
                y = v[p] -= MX();
            }
            z = v[n-1] ;
            y = v[0] -= MX();
            sum -= DELTA ;
        }
        status = 0;
    }

    #if( _BIG_ENDIAN )
    swap_bytes(v, n);
    #endif

    return status;
}


void swap_bytes( uint32_t *v, uint32_t n )
{
    uint32_t i;
    for( i=0; i<n; i++ )
    {
        v[i] = ( ( v[i] << 24 ) |
            ( ( v[i] << 8 ) & 0x00ff0000 ) |
            ( ( v[i] >> 8 ) & 0x0000ff00 ) |
            ( v[i] >> 24 ) );
    }
}
