//This file adapted from http://sites.google.com/site/drbobbobswebsite/cricut-gcode-interpreter
#include <cmath>
#include <vector>
#include <iostream>
#include <fstream>
#include <cstring>
#include "device_sim.hpp"

using namespace std;

//DEBUG LEVELS:
//0 - No debug output
//1 - StringConversion
//2 - Each line of GCODE as parsed
#define DEBUG 0

#include "gcode_common.h"

int main( int num_args, char * args[] )
{
if( num_args != 3 )
	{
	cout<<"Usage: "<<args[0]<<" output.bmp gcodefile.gcode"<<endl;
	exit(1);
	}

Device::CV_sim cutter( args[1] );

cutter.stop();
sleep(1);
cutter.start();
sleep(1);
parse_gcode( args[2], cutter );
cutter.stop();

return 0;
}
