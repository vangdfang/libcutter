#ifndef GCODE_HPP
#define GCODE_HPP

#include <cstring>
#include <string>
#include "device_c.hpp"
#include "types.h"

using namespace std;

#ifndef DEBUG_MSG
#define DEBUG_MSG
const string debug_msg = "\n"							\
     "Debug level goes from 0 (only show the most critical messages\n"	\
     "to 5 (output lots of extra debugging information). The default\n"	\
     "is 1 (only show error messages)\n";
#endif

// For historical reasons we use inches internally
const double MM_PER_INCH = 25.4;

enum debug_prio {
     crit = 0,
     err,
     warn,
     info,
     debug,
     extra_debug
};

class gcode
{
     // utility methods - methods, so that they can access private
     // members and stuff
     double doc_to_internal(double);
     double get_value(const string &, size_t *);
     xy get_xy(const string &, size_t *);
     xy get_vector(const string, size_t *);
     xy get_target(const string, size_t *);
     void debug_out(int, const string);
     void arc_segment_right(Device::Generic &, const xy &, double, double);
     void arc_segment(Device::Generic &, const xy &, double, double, double);

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

     void debug_out(enum debug_prio, string);

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

     // by default, only print critical stuff
     enum debug_prio _debug;

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
     inline void set_debug(enum debug_prio d)
	  {
	       _debug = d;
	  }

};
#endif
