#ifndef DEVICE_HPP
#define DEVICE_HPP

#include "serial_port.hpp"
#include "types.h"
#include <string>

typedef unsigned long _DWORD;

namespace Device
{
class Generic
{
	public:
		Generic();
		Generic( std::string aSerial );
		~Generic();
		void init( std::string aSerial );
		virtual inline const std::string device_name() { return "Virtual Device"; };
		virtual bool move_to(const xy &aPoint) = 0;
		virtual bool cut_to(const xy &aPoint) = 0;
		virtual bool curve_to(const xy &p0, const xy &p1, const xy &p2, const xy &p3) = 0;
		virtual bool start() = 0;
		virtual bool stop() = 0;
		inline bool is_connected() { return m_serial.is_open(); }

	protected:
		serial_port m_serial;
};
}
#endif
