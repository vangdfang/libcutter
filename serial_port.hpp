#ifndef SERIAL_PORT_HPP
#define SERIAL_PORT_HPP
#include <cstring>
#include <stdint.h>
#include <termios.h>
#include <linux/serial.h>

#define TIMING_GOAL (.001)
#define TIMING_CONSTRAINT (.00025)
#define BAUD_RATE B38400

class serial_port
{
public:
	serial_port();
	serial_port( const char * filename );
	~serial_port();

	bool is_open();
	void p_open ( const char * filename );
	void p_close();

	int p_get(); /* returns -1 on err*/
	bool p_put( uint8_t );

	std::size_t p_write( const uint8_t * data, std::size_t size );
	std::size_t p_read(  const uint8_t * data, std::size_t size );


protected:
	int  fd;
	bool stat;

	termios       oldtio;
	serial_struct oldsstruct;

	const double getTime();
};

#endif
