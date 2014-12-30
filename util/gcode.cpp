//This file adapted from http://sites.google.com/site/drbobbobswebsite/cricut-gcode-interpreter
#include <errno.h>
#include <cmath>
#include <vector>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include "device.hpp"

using namespace std;

#define MM_PER_INCH 25.4

bool metric = false;

int debug = 0;

xy curr_pos;

void debug_out(int debug_level, const string debug_text)
{
     if (debug >= debug_level)
     {
	  printf("%s\n", debug_text.c_str());
     }
}

double angle_between(const xy vec1, const xy vec2)
{
     printf("Angle between: (%f, %f) and (%f, %f): ", vec1.x, vec1.y,
	    vec2.x, vec2.y);
     // changing to using atan2
     double angle = atan2(vec2.y, vec2.x) - atan2(vec1.y, vec1.x);
     if (angle < 0)
	  angle = angle + 2*M_PI;
     printf("%f\n", angle/M_PI);
     return angle;
}

//Convert string arg to float in mm, then to inches, then add 1/2 inch for cutting surface
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

bool is_whitespace( char input )
{
     static const char whitespaces[]={' ','	','\n','\r', 0x00 };
     size_t i;

     for( i = 0; i < sizeof( whitespaces ); ++i )
     {
	  if( whitespaces[i] == input )
	  {
	       return true;
	  }

     }
     return false;
}

vector<string> split( string input )
{
     bool last_was_whitespace;
     size_t i;
     vector<string> retn;

     string emptystr="";

     last_was_whitespace = true;
     for( i = 0; i < input.size(); ++i )
     {
	  if( last_was_whitespace && !is_whitespace( input[i] ) )
	  {
	       retn.push_back(emptystr);
	       last_was_whitespace = false;
	       while( !is_whitespace( input[ i ] ) )
	       {
		    retn.back().push_back( input[ i ] );
		    i++;
	       }
	  }
	  
	  last_was_whitespace = is_whitespace( input[i] );
     }

     return retn;
}

bool parse_gcode( string file, Device::Generic & cutter)
{
     vector<string> line_parts;
     char * linebuf;
     ifstream inputfile( file.c_str() );
     bool pen_up;

     linebuf = (char*)malloc( 4096 );

     if( !inputfile )
     {
	  free( linebuf );
	  goto out;
     }

     pen_up = true;
     while( inputfile )
     {

	  memset(linebuf, 4096, 0);
	  inputfile.getline( linebuf, 4096 );
	  if( *linebuf == ';' )
	  {
	       continue;
	  }

	  line_parts = split( linebuf );

	  printf("G-code command: %s", linebuf);
	  if( strcmp( line_parts[0].c_str(), "G0") == 0 )
	  {
	       printf("\n");
	       // Rapid movement command
	       if( line_parts[1][0] == 'Z' ) 
	       {
		    // explicitly move the pen up or down
		    pen_up = (atof(line_parts[2].c_str()) > 0);
		    printf("Pen %s\n", pen_up ? "up" : "down");
	       } 
	       else if (line_parts[1][0] == 'X' ) 
	       {
		    // rapid move to the specified point
		    xy point = get_point_from_string(line_parts[2], line_parts[4]);
		    printf("Rapid move from (%f, %f) to (%f, %f)\n",
			   curr_pos.x, curr_pos.y, point.x, point.y);
		    cutter.move_to(point);
		    curr_pos = point;
		    printf("Current position: %f, %f\n", curr_pos.x, curr_pos.y);
	       } 
	       else
		    cout<<"Unknown G0 command "<<line_parts[1]<<endl;
	  } 
	  else if( strcmp( line_parts[0].c_str(), "G1") == 0 )
	  {
	       printf("\n");
	       //This is a movement command or something else
	       if( line_parts[1][0] == 'Z' )
	       {
		    //This is an up/down command
		    // Note that this is /not/ simply 1 or 0 or whatever,
		    // it's an absolute cutting height and should be interpreted
		    // as pen_up == true when it is positive and vice-versa.
		    pen_up = (atof(line_parts[2].c_str()) > 0);
		    printf("Pen %s\n", pen_up ? "up" : "down");
	       }
	       else if( line_parts[1][0] == 'X' )
	       {
		    xy point;

		    point = get_point_from_string(line_parts[2], line_parts[4]);
		    if( pen_up )
		    {
			 printf("Move from (%f, %f) to (%f, %f)\n",
				curr_pos.x, curr_pos.y, point.x, point.y);
			 cutter.move_to(point);
			 curr_pos = point;
			 printf("Current position: %f, %f\n", curr_pos.x, curr_pos.y);
		    }
		    else
		    {
			 printf("Cutting from (%f, %f) to (%f, %f)\n",
				curr_pos.x, curr_pos.y, point.x, point.y);
			 cutter.cut_to(point);
			 curr_pos = point;
			 printf("Current position: %f, %f\n", curr_pos.x, curr_pos.y);
		    }
	       }
	  } 
	  else if( strcmp( line_parts[0].c_str(), "G2") == 0)
	  {
	       printf("\n");
	       // Circle clockwise
	       if( line_parts[1][0] == 'Z' )
	       {
		    // No idea why this is showing up
		    cout<<"Unknown instance of Z command" <<endl;
	       }
	       else if( line_parts[1][0] == 'X' )
	       {
		    // this is implemented as a set of cubic bezier
		    // curves
		    xy target, center, cvec;
		    target = get_point_from_string(line_parts[2], line_parts[4]);
		    cvec = get_point_from_string(line_parts[6], line_parts[8]);
		    center.x = curr_pos.x + cvec.x;
		    center.y = curr_pos.y + cvec.y;
			     
		    // target vector and current vector
		    xy tvec;
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
	  }
	  else
	  {
	       printf(" . . . not handled\n");
	  }
     }
out:
     return true;
}

