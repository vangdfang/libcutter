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

//DEBUG LEVELS:
//0 - No debug output
//1 - StringConversion
//2 - Each line of GCODE as parsed
#define DEBUG 0

#include "gcode.hpp"

int main( int num_args, char * args[] )
{
     if( num_args != 3 )
     {
	  cout<<"Usage: "<<args[0]<<" output.bmp gcodefile.gcode"<<endl;
	  exit(1);
     }

     Device::CV_sim cutter( args[1] );
     gcode parser(args[2], cutter);

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
