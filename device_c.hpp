#ifndef DEVICE_C_HPP
#define DEVICE_C_HPP

#include <stdint.h>
#include "device.hpp"
#include "types.h"

typedef uint32_t ckey_type[4];

namespace Device
{
class C : public Device::Generic
{
	public:
		C();
		C( const std::string filename );
		/* virtual */ bool move_to(const xy &aPoint);
		/* virtual */ bool cut_to(const xy &aPoint);
		/* virtual */ bool curve_to(const xy &p0, const xy &p1, const xy &p2, const xy &p3);
		/* virtual */ bool start();
		/* virtual */ bool stop();
		inline void set_move_key( ckey_type k )
		{
			memcpy( m_move_key, k, sizeof(k)*4 );
		}
		inline void set_line_key( ckey_type k )
		{
			memcpy( m_line_key, k, sizeof(k)*4 );
		}
		inline void set_curve_key( ckey_type k )
		{
			memcpy( m_curve_key, k, sizeof(k)*4 );
		}
	private:
		inline int get_rand() const { return 12345; };
		xy convert_to_internal( const xy &input );
		bool do_command( const xy &pt, const ckey_type k );
		ckey_type m_move_key;
		ckey_type m_line_key;
		ckey_type m_curve_key;
};
}
#endif

