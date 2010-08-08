/***************************************************
 * This code based on code from 'correction to xtea'
 * David Wheeler djw3@cl.cam.ac.uk
 * Roger Needham rmn@cl.cam.ac.uk
 *
 * www.cix.co.uk/~klockstone/xxtea.pdf
 *
 * no longer from wikipedia
 * as this code is public domained
 * and wikipedia's isn't.
 **************************************************/
#include <assert.h>
#include <stdint.h>

int btea( long * v, long n , long * k )
{
    unsigned long z;
    unsigned long y;
    unsigned long sum;
    unsigned long e;

    //( sqrt( 5 ) - 1 ) / 2
    const unsigned long DELTA=0x9e3779b9 ;

    long p;
    long q;

    //Preinit check
    if( n == 0 )
    {
        assert( 0 );
        return 1;
    }

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
        return 0;
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
        return 0;
    }
    //This is handled above--but make the compiler happy
    return 1;
}
