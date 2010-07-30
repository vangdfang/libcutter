#include "serial_port.hpp"
#include <cstdio>
#include <sys/ioctl.h>
#include <linux/serial.h>
#include <cstdlib>
#include <fcntl.h>
#include <sys/time.h>
#include <unistd.h>
#include <cmath>
#include <string>
using std::size_t;
using namespace std;


serial_port::serial_port()
{
fd = -1;
}

serial_port::~serial_port()
{
p_close();
printf("port closed\n");
}

bool serial_port::is_open()
{
return fd >= 0;
}

serial_port::serial_port( const char * filename )
{
p_open( filename );
}

void serial_port::p_open( const char * filename )
{
termios newtio;
serial_struct sstruct;

fd = open( filename, O_RDWR | O_NOCTTY );
if( fd >= 0 )
	{
	tcgetattr( fd, &oldtio );
	memset( &newtio, 0x00, sizeof( newtio ) );
	newtio.c_cflag = BAUD_RATE | CS8 | CLOCAL | CREAD | CSTOPB;
	newtio.c_iflag = IGNPAR | ICRNL;
	newtio.c_oflag = 0;
	newtio.c_lflag = ICANON;
	tcflush(fd, TCIFLUSH);
	tcsetattr(fd,TCSANOW,&newtio);

	//ASYNC_SPD_MASK
	ioctl( fd, TIOCGSERIAL, &oldsstruct );
	sstruct = oldsstruct;
	sstruct.custom_divisor = sstruct.baud_base / 200000;
	sstruct.flags &= ~ASYNC_SPD_MASK;
	sstruct.flags |= ASYNC_SPD_MASK & ASYNC_SPD_CUST;
	printf("Divisor=%i\n", sstruct.custom_divisor );

	int r = ioctl( fd, TIOCSSERIAL, &sstruct );
	printf("r=%i\n",r);
	}

#ifdef SERIAL_PORT_DEBUG_MODE
#if SERIAL_PORT_DEBUG_MODE
char cmd[100];
sprintf(cmd, "stty -F %s", filename);
system(cmd);
sprintf(cmd, "setserial -a %s", filename);
system(cmd);
#endif
#endif
}

void serial_port::p_close()
{
if( fd >= 0 )
	{
	tcsetattr( fd, TCSANOW, &oldtio );
	ioctl( fd, TIOCSSERIAL, &oldsstruct );
	close( fd );
	fd = -1;
	}
}

int serial_port::p_get()
{
uint8_t buf;

if( read( fd, &buf, 1 ) != 1 )
	{
	return -1;
	}
else
	{
	return buf;
	}
}

bool serial_port::p_put( uint8_t buf )
{
write( fd, &buf, 1 );
fsync( fd );
}

size_t serial_port::p_write( const uint8_t * data, size_t size )
{
double s = getTime();
double t = getTime();
int    i;
int    count = 0;

for( i = 0; i < size; ++i )
	{
	if( count != 0 )
		{
		double dt = getTime() - t;
		double delta = 0;
		if( fabs( dt ) < TIMING_GOAL - TIMING_CONSTRAINT )
			{
			delta = TIMING_GOAL - TIMING_CONSTRAINT - fabs( dt );
			delta *= -1;
			}
		else if( fabs( dt ) > TIMING_GOAL + TIMING_CONSTRAINT )
			{
			delta = fabs( dt ) - TIMING_GOAL - TIMING_CONSTRAINT;
			}

		if( delta )
			{
			printf("WARNING:Serial Timing constrain violation by %f seconds\n", (float)dt );
			}
		}

	t = getTime();

	if( write( fd, data, 1 ) == 1 )
		{
		fsync( fd );
		data++;
		count++;
		}
	else
		{
		break;
		}

	}

printf("Took %f seconds to write\n", (float)(getTime()-s) );
return count;
}

size_t serial_port::p_read(  const uint8_t * data, size_t size )
{
return read( fd, (void*)data, size );
}

const double serial_port::getTime( void )
{
timeval tv;
gettimeofday( &tv, NULL );
return (double)( (long double)tv.tv_sec + (long double)tv.tv_usec / (long double) 1000000 );
}
