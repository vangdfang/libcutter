#ifndef GCODE_HPP
#define GCODE_HPP

#include <cstring>
#include "device.hpp"
#include "types.h"

bool parse_gcode( string file, Device::Generic & cutter);

#endif
