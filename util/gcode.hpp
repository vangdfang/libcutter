#ifndef GCODE_HPP
#define GCODE_HPP

#include <cstring>
#include <string>
#include <map>
#include "device.hpp"
#include "types.h"

using namespace std;

static const string debug_msg = "\n"							\
     "Debug level goes from 0 (only show the most critical messages\n"	\
     "to 5 (output lots of extra debugging information). The default\n"	\
     "is 1 (only show error messages)\n";

// For historical reasons we use inches internally
static const double MM_PER_INCH = 25.4;

enum debug_prio {
     crit = 0,
     err,
     warn,
     info,
     debug,
     extra_debug
};

namespace gcode_base
{
     void debug_out(enum debug_prio, const string);
     void set_debug(enum debug_prio);
}

// These are private utility classes
class line
{
     xy start, end;
     bool cut;

public:
     line(const xy &, const xy &, const bool);
     ~line();
     xy draw(Device::Generic &);
};

class bezier
{
     // start, end and control points
     xy start, cp1, cp2, end;

public:
     bezier(const xy &, const xy &, const xy &, const xy &);
     ~bezier();
     xy draw(Device::Generic &);
};

class arc
{
     // the g-code supplied values
     xy current, target;
     xy cvec;
     bool clockwise;

     // derived values defining the arc
     xy center;
     double radius, arcwidth, rotation;
     
     // we're implementing this as a 4-segment circle, so this is
     // appropriate
     const double k;
     bezier *segments[4];
     int cseg;
     double crot;

     // calculate the segments
     void segment(double swidth, double rot);

     // utility - ideally this would be done as part of an xy class,
     // but I don't want to  make such a large change right now
     double angle_between(const xy &, const xy &);
     double get_arcwidth(const xy &, const xy &);

public:
     arc(const xy &, const xy &, const xy &, const bool);
     arc(const xy &, double, double, double);
     ~arc();
     xy draw(Device::Generic &);
};

class gcode
{
     // parse methods
     static std::map<char,float> parse_gcode( std::string line );
     double doc_to_internal(double);
     xy get_xy(std::map<char,float> &);
     xy get_vector(std::map<char,float> &);
     xy get_target(std::map<char,float> &);

     // private stuff - methods so that they can access the private
     // methods and members
     void process_movement(std::map<char,float> &);
     void process_line(std::map<char,float> &);
     void process_clockwise_arc(std::map<char,float> &);
     void process_anticlockwise_arc(std::map<char,float> &);
     void process_g_code(std::map<char,float> &);
     void process_z_code(std::map<char,float> &);
     void process_line_number(std::map<char,float> &);
     void process_parens(std::map<char,float> &);
     void process_misc_code(std::map<char,float> &);

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
