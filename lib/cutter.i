%module cutter
%{
#include "types.h"
#include "device.hpp"
#include "device_c.hpp"
%}
%apply unsigned int { uint32_t }
%include "std_string.i"
%include "carrays.i"
%array_class(uint32_t, uint32Array)
%include "types.h"
%include "device.hpp"
%include "device_c.hpp"
