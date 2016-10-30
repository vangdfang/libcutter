//This file adapted from http://sites.google.com/site/drbobbobswebsite/cricut-gcode-interpreter
#include <errno.h>
#include <cmath>
#include <vector>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <stdexcept>
#include "device.hpp"
#include "gcode.hpp"

using namespace std;
using namespace gcode_base;

static const char *debug_strings[] = {
     "critical",
     "error",
     "warning",
     "information",
     "debug",
     "extra_debug"
};

static enum debug_prio _debug;

void gcode_base::debug_out(enum debug_prio debug_level, const string msg)
{
     if (debug_level <= _debug)
	  printf("%s\n", msg.c_str());
}

void gcode_base::set_debug(enum debug_prio d)
{
     char buf[256];

     _debug = d;
     snprintf(buf, sizeof(buf),
	      "Debugging level set to %s\n",
	      debug_strings[_debug]);
     debug_out(info, string(buf));
}

line::line(const xy &s, const xy &e, const bool c):
     start(s),
     end(e),
     cut(c)
{
     char buf[4096];
     snprintf(buf, sizeof(buf),
	      "New line: start (%f, %f), end (%f, %f) cut %s",
	      start.x, start.y, end.x, end.y, cut ? "true": "false");
     debug_out(extra_debug, string(buf));
}

line::~line()
{
}

xy line::draw(Device::Generic &cutter)
{
     char buf[4096];

     snprintf(buf, sizeof(buf),
	      "%s from (%f, %f) to (%f, %f)",
	      cut ? "Line" : "Rapid move",
	      start.x, start.y, end.x, end.y);
     debug_out(info, string(buf));

     if (cut)
	  cutter.cut_to(end);
     else
	  cutter.move_to(end);

     snprintf(buf, sizeof(buf),
	      "Current position: %f, %f",
	      end.x, end.y);
     debug_out(debug, string(buf));
     return end;
}

bezier::bezier(const xy &s, const xy &c1, const xy &c2, const xy &e):
     start(s),
     cp1(c1),
     cp2(c2),
     end(e)
{
     char buf[4096];
     snprintf(buf, sizeof(buf),
	      "New bezier from (%f, %f) to (%f, %f)",
	      start.x, start.y, end.x, end.y);
     debug_out(extra_debug, string(buf));
}

bezier::~bezier()
{
}

xy bezier::draw(Device::Generic &cutter)
{
     char buf[4096];

     snprintf(buf, sizeof(buf),
	      "Bezier segment from (%f, %f) to (%f, %f)",
	      start.x, start.y, end.x, end.y);
     debug_out(info, string(buf));
     cutter.curve_to(start, cp1, cp2, end);
     snprintf(buf, sizeof(buf),
	      "Current position: (%f, %f)",
	      end.x, end.y);
     debug_out(debug, string(buf));
     return end;
}

double arc::angle_between(const xy &vec1, const xy &vec2)
{
     double angle = atan2(vec2.y, vec2.x) - atan2(vec1.y, vec1.x);

     return angle;
}
double arc::get_arcwidth(const xy & vec1, const xy & vec2)
{
//     printf("vec1: (%f, %f); vec2: (%f, %f)\n",vec1.x, vec1.y,
//	    vec2.x, vec2.y);
     double a1 = atan2(vec2.y, vec2.x) - atan2(vec1.y, vec1.x);
//     printf("Arcwidth: %f", a1/M_PI);

     if (clockwise)
     {
//	  printf(" (clockwise) ");
	  if (a1 > 0)
	       a1 = a1 - 2*M_PI;
     }
     else
     {
//	  printf(" (anticlockwise) ");
	  if (a1 < 0)
	       a1 = a1 + 2*M_PI;
     }
//     printf(" Final: %f\n", a1/M_PI);
     return a1;
}

arc::arc(const xy &c, const xy &t, const xy &cv, const bool cw):
     current(c),
     target(t),
     cvec(cv),
     clockwise(cw),
     k((4.0/3.0)*(sqrt(2.0) - 1.0))
{
     char buf[4096];
     snprintf(buf, sizeof(buf),
	      "New %s arc from (%f, %f) to (%f, %f)",
	      clockwise ? "clockwise" : "anticlockwise",
	      current.x, current.y, target.x, target.y);
     debug_out(extra_debug, string(buf));

     cseg = 0;

     center.x = current.x + cvec.x;
     center.y = current.y + cvec.y;

     // target vector and current vector
     xy tvec;
     tvec.x = target.x - center.x;
     tvec.y = target.y - center.y;
     cvec.x = -(cvec.x);
     cvec.y = -(cvec.y);
     radius = sqrt(cvec.x*cvec.x + cvec.y*cvec.y);

     // We implement this using arcs of at most 90 degrees.
     // For arcs subtending more than 90 degrees we create
     // a list of 90 degree segments and then a sub-90
     // degree segment to complete the process.
     arcwidth = abs(get_arcwidth(cvec, tvec));

     // this is the angle between the start point and the
     // x-axis - we create the segments around the x-axis
     // and origin, then rotate them around the origin and 
     // translate to the defined center point.
     xy x_axis;
     x_axis.x = radius;
     x_axis.y = 0;
     crot = angle_between(x_axis, cvec);

     // we rotate each segment so that the start point of the segment
     // is at the same angle as the current point, then we increment
     // by the arcwidth of the segment.
     double srot = 0;
     double rem = arcwidth;
     while (rem > M_PI_2)
     {
	  // insert a 90 degree segment, rotated 
	  segment(M_PI_2, srot);
	  rem -= M_PI_2;
	  srot += M_PI_2;
     }
     // final arc segment
     segment(rem, srot);
}

arc::~arc()
{
}

xy arc::draw(Device::Generic &cutter)
{
     char buf[4096];

     snprintf(buf, sizeof(buf),
	      "Arc from (%f, %f) to (%f, %f)",
	      current.x, current.y, target.x, target.y);
     debug_out(info, string(buf));

     xy end;
     for(int i = 0; i < cseg; i++)
     {
	  if(segments[i] != NULL)
	       end = segments[i]->draw(cutter);
     }

     if (abs(end.x - target.x) > 0.000001 ||
	 abs(end.y - target.y) > 0.000001)
	  debug_out(warn, "Segment end points do not equal arc end points");
     snprintf(buf, sizeof(buf),
	      "Current position: (%f, %f)",
	      target.x, target.y);
     debug_out(debug, string(buf));
     return target;
}

// a single sub-90 degree segment. th0 is the total angle subtended by
// the arc, r is the arc radius, and rot is the rotation required to move
// the arc to its final position
//
// Reference: http://hansmuller-flex.blogspot.com.au/2011/04/approximating-circular-arc-with-cubic.html
// (accessed 2014-12-24)
void arc::segment(double swidth, double rot)
{
     char buf[4096];
     snprintf(buf, 4095,
	      "Arc segment: center (%f, %f), arc width: %f, radius %f, rotation: %f",
	      center.x, center.y, swidth/M_PI, radius, rot/M_PI);
     debug_out(debug, string(buf));
     xy pt1, pt2, pt3, pt4;
     double a = swidth/2;

     if (clockwise)
     {
	  // construct the arc segment around the x-axis
	  pt1.x = radius*cos(a);
	  pt1.y = radius*sin(a);
	  pt4.x = pt1.x;
	  pt4.y = -pt1.y;
	  // get the control points
	  pt3.x = pt4.x + k * tan(a) * pt1.y;
	  pt3.y = pt4.y + k * tan(a) * pt1.x;
	  pt2.x = pt3.x;
	  pt2.y = -pt3.y;
	  // this segment was created with pt1 pointing at a - it
	  // needs to be rotated by -a to point pt1 at the x axis,
	  // then by crot to point it at the current point. It then
	  // needs to be rotated by -rot: i.e. (-a + crot -rot), or
	  // crot - a - rot
	  rot = crot - a - rot;
     }
     else
     {
	  // construct the arc segment around the x-axis
	  pt4.x = radius*cos(a);
	  pt4.y = radius*sin(a);
	  pt1.x = pt4.x;
	  pt1.y = -pt4.y;
	  // get the control points
	  pt2.x = pt1.x + k * tan(a) * pt4.y;
	  pt2.y = pt1.y + k * tan(a) * pt4.x;
	  pt3.x = pt2.x;
	  pt3.y = -pt2.y;
	  // this time pt1 is pointing at -a, so the same set of
	  // rotations (except for using +rot rather than -rot, since
	  // this is anticlockwise) works out as (-(-a) + crot + rot),
	  // or rot + crot + a
	  rot = rot + crot + a;
     }

     // rotate to the right spot
     double cos_rot = cos(rot);
     double sin_rot = sin(rot);
     double a00 = cos_rot;
     double a01 = -sin_rot;
     double a10 = sin_rot;
     double a11 = cos_rot;

     double x, y;
     x = pt1.x * a00 + a01 * pt1.y;
     y = pt1.x * a10 + a11 * pt1.y;
     pt1.x = x;
     pt1.y = y;
     x = pt2.x * a00 + a01 * pt2.y;
     y = pt2.x * a10 + a11 * pt2.y;
     pt2.x = x;
     pt2.y = y;
     x = pt3.x * a00 + a01 * pt3.y;
     y = pt3.x * a10 + a11 * pt3.y;
     pt3.x = x;
     pt3.y = y;
     x = pt4.x * a00 + a01 * pt4.y;
     y = pt4.x * a10 + a11 * pt4.y;
     pt4.x = x;
     pt4.y = y;
     pt1.x = pt1.x + center.x;
     pt1.y = pt1.y + center.y;
     pt2.x = pt2.x + center.x;
     pt2.y = pt2.y + center.y;
     pt3.x = pt3.x + center.x;
     pt3.y = pt3.y + center.y;
     pt4.x = pt4.x + center.x;
     pt4.y = pt4.y + center.y;

     // and put it into production . . .
     segments[cseg++] = new bezier( pt1, pt2, pt3, pt4 );
}

//
// Parsing a line
//
// The g-code format is line-oriented, and consists of one or more
// g-code commands per line. Some commands simply set general state
// configuration, others take options and values. Comments start with
// a ';' character and continue to the end of the line, or are
// contained within paired (but not nested) parentheses.
//
// Commands can start with one of a number of characters, most often G
// and M, with all the important ones being G commands. We'll handle a
// small chunk of the G commands, one or two of the M commands, and a
// couple of other miscellaneous things.
//
// G commands that will be handled are 0 (rapid movement), 1 (line), 2
// (clockwise circular arc), 3 (anticlockwise circular arc), 20 and 21
// (inches or millimeters mode), 90 and 91 (absolute or incremental
// positioning). M commands that will be handled are 0 and 2
// (compulsory stop, and end program). Finally we'll recognise N-code
// line numbers, and we'll obviously parse the XYZ and IJK options
// that are associated with the G commands.
//
// The actual implementation will be a recursive, character oriented
// parser - read a character, and depending on its value call a
// function to process subsequent characters.
//
// Each function will return any remaining substring that hasn't been
// processed - i.e. once it's parsed a command to completion it will
// return the remaining unparsed string.
//

gcode::gcode(Device::Generic & c):
     cutter(c)
{
     filename = string("");
     pen_up = true;
     metric = true;
     absolute = true;
}

gcode::gcode(const std::string & fname, Device::Generic & c):
     cutter(c)
{
     filename = fname;
     pen_up = true;
     metric = true;
     absolute = true;
}

gcode::~gcode()
{
}

void gcode::set_input(const std::string & fname)
{
     filename = fname;
}

void gcode::set_cutter(Device::Generic & c)
{
     cutter = c;
}

double gcode::doc_to_internal(double val)
{
     if(metric)
	  return (val/MM_PER_INCH);
     return val;
}

char gcode::get_command(const string & input, size_t *rem)
{
     int offset = 0;
     char command;

     while(isspace(input[offset]))
	  offset++;
     command = input[offset];
     *rem = offset + 1;

     return command;
}

int gcode::get_code(const string & input, size_t *rem)
{
     char *end;
     const char *tmp;
     int retval;

     tmp = input.c_str();
     retval = strtol(tmp, &end, 10);
     *rem = (end - tmp) + 1;
     return retval;
}

double gcode::get_value(const string & input, size_t *rem)
{
     char *end;
     const char *tmp;
     double retval;

     tmp = input.c_str();
     retval = strtod(tmp, &end);
     retval = doc_to_internal(retval);
     *rem = (end - tmp) + 1;
     return retval;
}

xy gcode::get_xy(const string & input, size_t *rem)
{
     size_t tmp;
     size_t offset = 0;
     char command;
     xy target;
     double x, y;

     command = get_command(input, &tmp);
     offset += tmp;
     if(command != 'X' && command != 'I')
     {
	  string msg = "Expected first part, got: ";
	  msg.append(input);
	  throw msg;
     }
     x = get_value(input.substr(offset), &tmp);
     offset += tmp;
     command = get_command(input.substr(offset), &tmp);
     offset += tmp;
     if(command != 'Y' && command != 'J')
     {
	  string msg = "Expected second part, got: ";
	  msg.append(input.substr(offset));
	  throw msg;
     }
     y = get_value(input.substr(offset), &tmp);
     offset += tmp;
     *rem = offset;
     target.x = x;
     target.y = y;
     return target;
}

xy gcode::get_vector(const string input, size_t *rem)
{
     char command;
     size_t offset = 0;
     size_t tmp;

     command = get_command(input, &tmp);
     offset += tmp;
     if(command == 'I')
     {
	  xy t = get_xy(input, &tmp);
	  offset += tmp;
	  *rem = offset;
	  return t;
     }
     string msg = "Expected vector, got: ";
     msg.append(input);
     throw msg;
}

xy gcode::get_target(const string input, size_t *rem)
{
     char command;

     command = get_command(input, rem);
     if(command == 'X')
	  return get_xy(input, rem);
     string msg = "Expected xy, got: ";
     msg.append(input);
     throw msg;
}

void gcode::process_movement(string input)
{
     // rapid movement to target point
     //
     size_t rem = 0;
     char command;
     char buf[4096];

     command = get_command(input, &rem);
     if(command == 'Z')
     {
	  double z = get_value(input.substr(rem), &rem);
	  snprintf(buf, 4095, "Pen %s", (z >= 0) ? "up":"down");
	  debug_out(debug, string(buf));
	  if(z >= 0)
	       raise_pen();
	  else
	       lower_pen();
     }
     else if(command == 'X')
     {
	  xy target;

	  target = get_target(input, &rem);
	  line *l = new line(curr_pos, target, false);
	  curr_pos = l->draw(cutter);
     }
     else
     {
	  string msg = "Unknown G0 command: ";
	  msg.append(input);
	  throw msg;
     }
     parse_line(input.substr(rem));
}

void gcode::process_line(string input)
{
     // cut from curr_pos to target_point

     size_t offset = 0;
     size_t rem;
     char command;
     char buf[4096];

     command = get_command(input, &rem);
     offset += rem;
     if(command == 'Z')
     {
	  double z = get_value(input.substr(rem), &rem);
	  offset += rem;
	  snprintf(buf, 4095, "Pen %s", (z >= 0) ? "up":"down");
	  debug_out(debug, string(buf));
	  if(z >= 0)
	       raise_pen();
	  else
	       lower_pen();
     }
     else if(command == 'X')
     {
	  xy target;

	  target = get_target(input, &rem);
	  offset += rem;
	  line *l = new line(curr_pos, target, true);
	  curr_pos = l->draw(cutter);
     }
     else
     {
	  string msg = "Unknown G1 command: ";
	  msg.append(input);
	  throw msg;
     }
     parse_line(input.substr(offset));
}

void gcode::process_clockwise_arc(string input)
{
     // cut in a clockwise circular arc from curr_pos to target around
     // a center point defined by the vector (i, j) from the current
     // position

     size_t offset = 0;
     size_t rem;
     char command;

     command = get_command(input, &rem);
     offset += rem;
     if(command == 'Z')
     {
	  string msg = "Unexpected Z command: ";
	  msg.append(input);
	  throw msg;
     }
     else if(command == 'X')
     {
	  xy target, cvec;

	  debug_out(debug, "Processing clockwise arc");
	  target = get_target(input, &rem);
	  offset += rem;
	  cvec = get_vector(input.substr(rem), &rem);
	  offset += rem;

	  arc *a = new arc(curr_pos, target, cvec, true);
	  curr_pos = a->draw(cutter);
     }
     parse_line(input.substr(offset));
}

void gcode::process_anticlockwise_arc(string input)
{
    // cut in an anticlockwise circular arc from curr_pos to target
    // around a center point defined by the vector (i, j) from the
    // current position

    size_t offset = 0;
    size_t rem;
    char command;

    command = get_command(input, &rem);
    offset += rem;
    if(command == 'Z')
    {
	 string msg = "Unexpected Z command: ";
	 msg.append(input);
	 throw msg;
    }
    else if(command == 'X')
    {
	 xy target, cvec;

	 debug_out(debug, "Processing anticlockwise arc");
	 target = get_target(input, &rem);
	 offset += rem;
	 cvec = get_vector(input.substr(rem), &rem);
	 offset += rem;
	 arc *a = new arc(curr_pos, target, cvec, false);
	 curr_pos = a->draw(cutter);

     }
     parse_line(input.substr(offset));
}

void gcode::process_g_code(string input)
{
     string val;
     size_t rem = 0;

     int code = get_code(input, &rem);
     char buf[4096];
     snprintf(buf, 4095, "Processing G code: %d", code);
     debug_out(debug, string(buf));
     switch(code)
     {
     case 0:
	  // rapid movement to target point
	  //
	  process_movement(input.substr(rem));
	  break;
     case 1:
	  // cut a line from curr_pos to target point
	  process_line(input.substr(rem));
	  break;
     case 2:
	  // clockwise circular arc from curr_pos to target, around
	  // the center point specified by the vector (i, j)
	  process_clockwise_arc(input.substr(rem));
	  break;
     case 3:
	  // anticlockwise circular arc, as per case 2
	  process_anticlockwise_arc(input.substr(rem));
	  break;
     case 20:
	  // input values are in inches
	  debug_out(info, "Switching to imperial units");
	  metric = false;
	  break;
     case 21:
	  // input values are in millimeters
	  debug_out(info, "Switching to metric units");
	  metric = true;
	  break;
     case 90:
	  // values are absolute
	  debug_out(info, "Using absolute coordinates");
	  absolute = true;
	  break;
     case 91:
	  // values are relative to the current point
	  // not supported at the moment, so we do nothing
	  debug_out(info, "Relative coordinates requested but not supported");
	  break;
     default:
	  string msg = "Unhandled G command: ";
	  msg.append(input);
	  debug_out(debug, msg);
	  break;
     }
     parse_line(input.substr(rem));
}

void gcode::process_line_number(string input)
{
     int offset = 1;

     debug_out(debug, "Skipping line number");
     // skip any white space between the N and the rest of the line
     while(isspace(input[offset]))
	  offset++;
     // skip the digits following
     while(isdigit(input[offset]))
	  offset++;
     // and skip the subsequent white space . . .
     while(isspace(input[offset]))
	  offset++;

     parse_line(input.substr(offset));
}

void gcode::process_parens(string input)
{
     int offset = 1;

     debug_out(debug, "Skipping parentheses");
     // skip anything not a corresponding ')'
     while(input[offset] != ')')
	  offset++;
     offset++;

     parse_line(input.substr(offset));
}

void gcode::process_misc_code(string input)
{
     size_t rem = 0;

     int code = get_code(input, &rem);
     char buf[4096];
     snprintf(buf, 4095, "Processing M code: %d", code);
     debug_out(debug, string(buf));
     switch(code)
     {
     case 0:
     case 1:
     case 2:
	  // stop the program
	  debug_out(info, "Program halted");
	  throw false;
	  break;
     default:
	  string msg = "Unhandled M command ";
	  msg.append(input);
	  debug_out(debug, msg);
	  break;
     }

     parse_line(input.substr(rem));
}

void gcode::parse_line(string input)
{
     size_t rem = 0;
     char buf[4096];
     snprintf(buf, 4095, "Processing line: %s", input.c_str());
     debug_out(extra_debug, string(buf));
     if(input[0] == '\n')
	  throw true;

     char command = get_command(input, &rem);
     switch(command)
     {
     case 'N':
	  process_line_number(input.substr(rem));
	  break;

     case 'G':
	  process_g_code(input.substr(rem));
	  break;

     case 'M':
	  process_misc_code(input.substr(rem));
	  break;

     case '(':
	  process_parens(input.substr(rem));
	  break;

     case ';':
	  break;

     case '\n':
     case '\0':
	  break;

     default:
	  string msg = "Unhandled command ";
	  msg.append(input);
	  debug_out(debug, msg);
	  break;
     }
}

void gcode::parse_file(void)
{
     ifstream infile(filename.c_str());
     string line;
     char buf[4096];

     while(infile)
     {
	  std::getline(infile, line);
	  try
	  {
	       parse_line(line);
	  }
	  catch(string msg)
	  {
	       snprintf(buf, 4095, "%s", msg.c_str());
	       debug_out(err, string(buf));
	  }
	  catch(const std::out_of_range& oor)
	  {
	       // sadly, this seems to be the most reliable way to
	       // tell that we've reached the end of the line
	       debug_out(extra_debug, "Got out of range error");
	  }
	  catch(bool completed)
	  {
	       // this flags a stop command
	       if(!completed)
		    return;
	       // otherwise the line is done
	  }
     }
     debug_out(info, "Parse complete");
     return;
}
