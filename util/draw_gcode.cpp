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
	  cout<<"Usage: "<<args[0]<<" /dev/ttyCricut0 gcodefile.gcode"<<endl;
	  exit(1);
     }

     Device::C cutter( args[1] );
     gcode parser( args[2], cutter );

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
