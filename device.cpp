#include "device.hpp"

namespace Device
{
	Generic::Generic() : m_serial()
	{
	}

	Generic::Generic( std::string aSerial ) : m_serial()
	{
		init( aSerial );
	}

	Generic::~Generic()
	{
		m_serial.p_close();
	}

	void Generic::init( std::string aSerial )
	{
		m_serial.p_open( aSerial );
	}
}
