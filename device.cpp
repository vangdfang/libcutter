#include "device.hpp"

namespace Device
{
    Generic::Generic()
    {
    }

    Generic::Generic( std::string aSerial )
    {
        init( aSerial );
    }

    Generic::~Generic()
    {
    }

    void Generic::init( std::string aSerial )
    {
    }
}
