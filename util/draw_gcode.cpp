//This file adapted from http://sites.google.com/site/drbobbobswebsite/cricut-gcode-interpreter

#include <cmath>
#include <vector>
#include <iostream>
#include <fstream>
#include <cstring>
#include "device_c.hpp"
#include "keys.h"

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
	cout<<"Usage: "<<args[0]<<" /dev/ttyCricut0 gcodefile.gcode"<<endl;
	exit(1);
	}

Device::C cutter( args[1] );

cutter.stop();
cutter.start();

ckey_type move_key={MOVE_KEY_0, MOVE_KEY_1, MOVE_KEY_2, MOVE_KEY_3 };
cutter.set_move_key(move_key);

ckey_type line_key={LINE_KEY_0, LINE_KEY_1, LINE_KEY_2, LINE_KEY_3 };
cutter.set_line_key(line_key);



parse_gcode( args[2], cutter );
cutter.stop();

return 0;
}
