//This file adapted from http://sites.google.com/site/drbobbobswebsite/cricut-gcode-interpreter
#include <cmath>
#include <vector>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include "device.hpp"

using namespace std;

#define MM_PER_INCH 25.4

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

#if( DEBUG >= 2 )
	cout<<"Split found "<<retn.size()<<" things"<<endl;
#endif

return retn;
}

bool parse_gcode( string file, Device::Generic & cutter)
{
	vector<string> line_parts;
	char * linebuf;
	ifstream inputfile( file.c_str() );
	bool pen_up;

	pen_up = true;
	while( inputfile )
	{
		linebuf = (char*)malloc( 4096 );
		inputfile.getline( linebuf, 4096 );
		#if( DEBUG >= 2 )
			cout << "GCODE:"<<linebuf<<endl;
		#endif

		if( !inputfile )
			{
			#if DEBUG>=1
				cout<<"EndOfInput"<<endl;
			#endif
			free( linebuf );
			break;
			}

		if( *linebuf == ';' )
			{
			#if( DEBUG >=2 )
				cout<<"Skipping comment"<<endl;
			#endif
			free( linebuf );
			continue;
			}

		line_parts = split( linebuf );
		#if DEBUG >= 2
			cout<<"Found "<<line_parts.size()<<" tokens"<<endl;
		#endif
		free( linebuf );

		if( strcmp( line_parts[0].c_str(), "G1") == 0 )
		{
			//This is a movement command or something else
			if( line_parts[1][0] == 'Z' )
			{
				//This is an up/down command
				pen_up = line_parts[1][1] == '1';
			}
			else if( line_parts[1][0] == 'X' )
			{
				xy point;

				#if DEBUG >= 1
					cout<<"COORDS: XPART:"<<line_parts[1]<<" YPART:"<<line_parts[2]<<endl;
				#endif

				//Convert string arg to float in mm, then to inches, then add 1/2 inch for cutting surface
				point.x = ( atof( line_parts[1].c_str() + 1 ) / MM_PER_INCH) + .5;
				point.y = ( atof( line_parts[2].c_str() + 1 ) / MM_PER_INCH) + .5;
				if( pen_up )
				{
					cout<<"Moving with pen up " << point.x << "," << point.y <<endl;
					cutter.move_to(point);
				}
				else
				{
					cout<<"Moving with pen down " << point.x << "," << point.y <<endl;
					cutter.cut_to(point);
				}
			}
		}
	}
	return true;
}
