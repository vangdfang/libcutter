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

int debug = 0;

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

void debug_out(int debug_level, const string debug_text)
{
     if (debug >= debug_level)
     {
	  printf("%s\n", debug_text.c_str());
     }
}

double angle_between(const xy vec1, const xy vec2)
{
     // changing to using atan2
     double angle = atan2(vec2.y, vec2.x) - atan2(vec1.y, vec1.x);
     if (angle < 0)
	  angle = angle + 2*M_PI;
     return angle;
}

//Convert string arg to float in mm, then to inches
xy get_point_from_string(const string part1, const string part2)
{
     xy point;

     point.x = atof( part1.c_str()) / MM_PER_INCH;
     point.y = atof( part2.c_str()) / MM_PER_INCH;

     return point;
}

// we can treate right angle segments differently, because they can be
// done with pre-calculated values that are then scaled, rotated and
// translated
void arc_segment_right(Device::Generic & cutter, const xy & center,
		       double r, double rot)
{
     printf("Right arc segment: center (%f, %f) radius %f, rotation: %f\n",
	    center.x, center.y, r, rot/M_PI);
     double k = (4.0/3.0)*(sqrt(2.0) - 1.0);
     xy pt1, pt2, pt3, pt4;
     rot = rot - M_PI_2;

     pt1.x = 0;
     pt1.y = r;
     pt2.x = k*r;
     pt2.y = r;
     pt3.x = r;
     pt3.y = k*r;
     pt4.x = r;
     pt4.y = 0;
     
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
     printf("Cutter: curve from (%f, %f) to (%f, %f), ctrl pts (%f, %f) and (%f, %f)\n",
	    pt1.x, pt1.y, pt4.x, pt4.y, pt2.x, pt2.y, pt3.x, pt3.y);
     cutter.curve_to( pt1, pt2, pt3, pt4 );
}

// a single sub-90 degree segment. th0 is the total angle subtended by
// the arc, r is the arc radius, and rot is the rotation required to move
// the arc to its final position
//
// Reference: http://hansmuller-flex.blogspot.com.au/2011/04/approximating-circular-arc-with-cubic.html
// (accessed 2014-12-24)
void arc_segment( Device::Generic & cutter, const xy & center,
		  double th0, double r, double rot)
{
     printf("Arc segment: center (%f, %f), arc width: %f, radius %f, rotation: %f\n",
	    center.x, center.y, th0/M_PI, r, rot/M_PI);
     if ( abs((th0 - M_PI_2)) < 0.00000000001) {
	  return arc_segment_right(cutter, center, r, rot);
     }
     xy pt1, pt2, pt3, pt4;
     double k = (4.0/3.0)*(sqrt(2.0) - 1.0);
     double a = th0/2;
     rot = rot - a;

     // construct the arc segment around the x-axis
     pt1.x = r*cos(a);
     pt1.y = r*sin(a);
     pt4.x = pt1.x;
     pt4.y = -pt1.y;

     // get the control points
     pt3.x = pt4.x + k * tan(a) * pt1.y;
     pt3.y = pt4.y + k * tan(a) * pt1.x;
     pt2.x = pt3.x;
     pt2.y = -pt3.y;

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
     printf("Cutter: curve from (%f, %f) to (%f, %f), ctrl pts (%f, %f) and (%f, %f)\n",
	    pt1.x, pt1.y, pt4.x, pt4.y, pt2.x, pt2.y, pt3.x, pt3.y);
     cutter.curve_to( pt1, pt2, pt3, pt4 );
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

char get_command(const string & input, size_t *rem)
{
     int offset = 0;
     char command;

     while(isspace(input[offset]))
	  offset++;
     command = input[offset];
     *rem = offset + 1;

     return command;
}

int get_code(const string & input, size_t *rem)
{
     char *end;
     const char *tmp;
     int retval;

     tmp = input.c_str();
     retval = strtol(tmp, &end, 10);
     *rem = (end - tmp) + 1;
     return retval;
}

double get_value(const string & input, size_t *rem)
{
     char *end;
     const char *tmp;
     double retval;

     tmp = input.c_str();
     retval = strtod(tmp, &end);
     *rem = (end - tmp) + 1;
     return retval;
}     
	  
xy get_xy(const string & input, size_t *rem)
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

xy get_vector(string input, size_t *rem)
{
     char command;

     command = get_command(input, rem);
     if(command == 'I')
	  return get_xy(input, rem);
     string msg = "Expected vector, got: ";
     msg.append(input);
     throw msg;
}

xy get_target(string input, size_t *rem)
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
     
     command = get_command(input, &rem);
     if(command == 'Z')
     {
	  double z = get_value(input.substr(rem), &rem);
	  printf("Pen %s\n", (z >= 0) ? "up":"down");
	  if(z >= 0)
	       raise_pen();
	  else
	       lower_pen();
     }
     else if(command == 'X')
     {
	  xy target;

	  target = get_target(input, &rem);

	  printf("Rapid move from (%f, %f) to (%f, %f)\n",
		 curr_pos.x, curr_pos.y, target.x, target.y);
	  cutter.move_to(target);
	  curr_pos = target;
	  printf("Current position: %f, %f\n", curr_pos.x, curr_pos.y);
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

     size_t rem = 0;
     char command;

     command = get_command(input, &rem);
     if(command == 'Z')
     {
	  double z = get_value(input.substr(rem), &rem);
	  printf("Pen %s\n", (z >= 0) ? "up":"down");
	  if(z >= 0)
	       raise_pen();
	  else
	       lower_pen();
     }
     else if(command == 'X')
     {
	  xy target;

	  target = get_target(input, &rem);

	  printf("Cutting from (%f, %f) to (%f, %f)\n",
		 curr_pos.x, curr_pos.y, target.x, target.y);
	  cutter.cut_to(target);
	  curr_pos = target;
	  printf("Current position: %f, %f\n", curr_pos.x, curr_pos.y);
     }
     else
     {
	  string msg = "Unknown G1 command: ";
	  msg.append(input);
	  throw msg;
     }
     parse_line(input.substr(rem));
}

void gcode::process_clockwise_arc(string input)
{
     // cut in a clockwise circular arc from curr_pos to target around
     // a center point defined by the vector (i, j) from the current
     // position

     size_t rem = 0;
     char command;
     
     command = get_command(input, &rem);
     if(command == 'Z')
     {
	  string msg = "Unexpected Z command: ";
	  msg.append(input);
	  throw msg;
     }
     else if(command == 'X')
     {
	  xy target, cvec;
	  xy center, tvec;
	  
	  target = get_target(input, &rem);
	  cvec = get_vector(input.substr(rem), &rem);

	  center.x = curr_pos.x + cvec.x;
	  center.y = curr_pos.y + cvec.y;
			     
	  // target vector and current vector
	  tvec.x = target.x - center.x;
	  tvec.y = target.y - center.y;
	  cvec.x = -(cvec.x);
	  cvec.y = -(cvec.y);
	  double radius = sqrt(cvec.x*cvec.x + cvec.y*cvec.y);

	  // We implement this using arcs of at most 90 degrees.
	  // For arcs subtending more than 90 degrees we create
	  // a list of 90 degree segments and then a sub-90
	  // degree segment to complete the process.
	  double arcwidth = angle_between(tvec, cvec);
	  // this is the angle between the start point and the
	  // x-axis - we create the segments around the x-axis
	  // and origin, then rotate them around the origin and 
	  // translate to the defined center point.
	  xy x_axis;
	  x_axis.x = radius;
	  x_axis.y = 0;
	  double trot = angle_between(x_axis, cvec);
			     
	  double rem = arcwidth;
	  // this value reduces for each segment, so that we
	  // rotate the first segment to point at curr_pos
	  // then the next one to point at the end of the first
	  // segment, and so on.
	  double srot = trot;
	  while (abs(rem) > M_PI_2 && abs(rem) < 2*M_PI)
	  {
	       // insert a 90 degree segment, rotated 
	       arc_segment_right(cutter, center, radius, srot);
	       rem = rem - M_PI_2;
	       arcwidth = rem;
	       srot = srot - M_PI_2;
	  }
	  // final arc segment
	  arc_segment(cutter, center, arcwidth, radius, srot);
	  curr_pos = target;
	  printf("Current position: %f, %f\n", curr_pos.x, curr_pos.y);
     }
     parse_line(input.substr(rem));
	  
}

void gcode::process_anticlockwise_arc(string input)
{
}

void gcode::process_g_code(string input)
{
     string val;
     size_t rem = 0;

     int code = get_code(input, &rem);
     printf("Processing G code: %d\n", code);
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
	  metric = false;
	  break;
     case 21:
	  // input values are in millimeters
	  metric = true;
	  break;
     case 90:
	  // values are absolute
	  absolute = true;
	  break;
     case 91:
	  // values are relative to the current point
	  // not supported at the moment, so we do nothing
	  break;
     default:
	  string msg = "Unhandled G command: ";
	  msg.append(input);
	  throw msg;
	  break;
     }
     parse_line(input.substr(rem));
}

void gcode::process_line_number(string input)
{
     int offset = 1;

     printf("Skipping line number\n");
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

     printf("Skipping parentheses\n");
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
     printf("Processing M code: %d\n", code);
     switch(code)
     {
     case 0:
     case 1:
     case 2:
	  // stop the program
	  throw false;
	  break;
     default:
	  string msg = "Unhandled M command ";
	  msg.append(input);
	  throw msg;
	  break;
     }
     
     parse_line(input.substr(rem));
}

void gcode::parse_line(string input)
{
     size_t rem = 0;
     printf("Processing line: %s\n", input.c_str());
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

     default:
	  string msg = "Unhandled command ";
	  msg.append(input);
	  throw msg;
	  break;
     }
}

void gcode::parse_file(void)
{
     ifstream infile(filename.c_str());
     string line;

     while(infile)
     {
	  std::getline(infile, line);
	  try
	  {
	       parse_line(line);
	  }
	  catch(string msg)
	  {
	       printf("%s\n", msg.c_str());
	  }
	  catch(const std::out_of_range& oor)
	  {
	       // sadly, this seems to be the most reliable way to
	       // tell that we've reached the end of the line
	       printf("Got out of range error\n");
	  }
	  catch(bool completed)
	  {
	       // this flags a stop command
	       if(!completed)
		    return;
	       // otherwise the line is done
	  }
     }
     printf("Parse complete\n");
     return;
}
	  
