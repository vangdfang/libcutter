#ifndef GCODE_HPP
#define GCODE_HPP

#include <cstring>
#include "device.hpp"
#include "types.h"

xy get_point_from_string(const string, const string);
bool parse_gcode( string file, Device::Generic & cutter);

#endif
