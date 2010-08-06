//This code from wikipedia
#include <stdint.h>

void btea(unsigned int *v, int n, unsigned int const k[4])
{
//Constants
#define DELTA 0x9e3779b9
#define MX ((z>>5^y<<2) + (y>>3^z<<4)) ^ ((sum^y) + (k[(p&3)^e] ^ z));

//Variables
unsigned int y;
unsigned int z;
unsigned int sum;
unsigned int p;
unsigned int rounds;
unsigned int e;

if (n > 1)
	{          /* Coding Part */
	rounds = 6 + 52/n;
	sum = 0;
	z = v[n-1];
	do
		{
		sum += DELTA;
		e = (sum >> 2) & 3;
		for (p=0; p<n-1; p++)
			{
			y = v[p+1], z = v[p] += MX;
			}
		y = v[0];
		z = v[n-1] += MX;
		} while (--rounds);
	}
else if (n < -1)
	{  /* Decoding Part */
	n = -n;
	rounds = 6 + 52/n;
	sum = rounds*DELTA;
	y = v[0];
	do {
        e = (sum >> 2) & 3;
        for (p=n-1; p>0; p--)
			{
			z = v[p-1], y = v[p] -= MX;
			}
		z = v[n-1];
		y = v[0] -= MX;
		}while ((sum -= DELTA) != 0);
	}
}

