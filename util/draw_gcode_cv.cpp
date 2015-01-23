//This file adapted from http://sites.google.com/site/drbobbobswebsite/cricut-gcode-interpreter
#include <cmath>
#include <vector>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include "device_sim.hpp"

#include <unistd.h>

using namespace std;

#include "gcode.hpp"

void usage(char *progname)
{
     printf("Usage: %s [-d debug_level] <output file> <gcode file>\n",
	    progname);
     printf("Output is a bmp format file\n");
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
	       // get the subsequent integer debug priority
	       // value
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


     Device::CV_sim cutter( args[arg_start++] );
     gcode parser(args[arg_start++], cutter);
     gcode_base::set_debug(d);

     cutter.stop();
     cutter.start();
     try
     {
	       parser.parse_file();
     }
     catch(...)
     {
	  printf("Unhandled exception\n");
     }
     cutter.stop();
     sleep(1);
     return 0;
}
