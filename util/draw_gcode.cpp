//This file adapted from http://sites.google.com/site/drbobbobswebsite/cricut-gcode-interpreter

#include <cmath>
#include <vector>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include "device_c.hpp"
#include "keys.h"

using namespace std;

#include "gcode.hpp"

void usage(char *progname)
{
     printf("Usage: %s [-d debug_level] <device file> <gcode file>\n",
	    progname);
     printf("%s\n", debug_msg.c_str());
     exit(1);
}

int main( int num_args, char * args[] )
{
     int arg_start = 1;
     enum debug_prio d = err;

     if( num_args == 5 )
     {
	  if(strncmp(args[1], "-d", 2) == 0)
	  {
	       d = (enum debug_prio)strtol(args[2], NULL, 10);
	       arg_start = 3;
	  }
	  else
	       usage(args[0]);
     }
     else if( num_args == 3 )
	  arg_start = 1;
     else
	  usage(args[0]);

     Device::C cutter( args[arg_start++] );
     gcode parser( args[arg_start++], cutter );
     gcode_base::set_debug(d);

     cutter.stop();
     cutter.start();

     ckey_type move_key={MOVE_KEY_0, MOVE_KEY_1, MOVE_KEY_2, MOVE_KEY_3 };
     cutter.set_move_key(move_key);

     ckey_type line_key={LINE_KEY_0, LINE_KEY_1, LINE_KEY_2, LINE_KEY_3 };
     cutter.set_line_key(line_key);

     ckey_type curve_key={CURVE_KEY_0, CURVE_KEY_1, CURVE_KEY_2, CURVE_KEY_3 };
     cutter.set_curve_key(curve_key);

     try
     {
	       parser.parse_file();
     }
     catch(...)
     {
	  printf("Unhandled exception");
     }

     cutter.stop();

     return 0;
}
