#ifndef GCODE_HPP
#define GCODE_HPP

#include <cstring>
#include <string>
#include "device_c.hpp"
#include "types.h"

using namespace std;

// For historical reasons we use inches internally
const double MM_PER_INCH = 25.4;

class gcode
{
     // private stuff - methods so that they can access the private
     // methods and members
     void process_movement(string);
     void process_line(string);
     void process_clockwise_arc(string);
     void process_anticlockwise_arc(string);
     void process_g_code(string);
     void process_line_number(string);
     void process_parens(string);
     void process_misc_code(string);

     inline void raise_pen(void)
	  {
	       pen_up = true;
	  }
     inline void lower_pen(void)
	  {
	       pen_up = false;
	  }
     inline void set_metric(bool m)
	  {
	       metric = m;
	  }
     inline void set_absolute(bool a)
	  {
	       absolute = a;
	  }

     Device::Generic & cutter;
     std::string filename;
     xy curr_pos;

     bool pen_up;
     bool metric;
     bool absolute;

public:
     gcode(Device::Generic &);
     gcode( const  std::string &, Device::Generic & );
     ~gcode();
     
     void set_input(const std::string &);
     void set_cutter(Device::Generic &);

     void parse_file(void);
     void parse_line(string);

     inline bool is_pen_up(void)
	  {
	       return pen_up;
	  }
     inline bool is_metric(void)
	  {
	       return metric;
	  }
     inline bool is_absolute(void)
	  {
	       return absolute;
	  }
};
#endif
