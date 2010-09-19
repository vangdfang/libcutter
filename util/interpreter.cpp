#include <string>
#include <fstream>
#include <iostream>
#include <cstdlib>
using namespace std;

#include "keys.h"
#include "device_c.hpp"

int main( int numArgs, char * args[] )
{
    if( numArgs != 3 )
    {
        cout<<"Usage: "<<args[0]<<" /dev/serial/port filename"<<endl;
        exit(1);
    }

    ifstream inputfile(args[2] );
    if(!inputfile )
    {
        cout<<"Error opening file"<<endl;
        exit(2);
    }

    Device::C c(args[1]);
    ckey_type move_key={MOVE_KEY_0, MOVE_KEY_1, MOVE_KEY_2, MOVE_KEY_3 };
    c.set_move_key(move_key);

    ckey_type line_key={LINE_KEY_0, LINE_KEY_1, LINE_KEY_2, LINE_KEY_3 };
    c.set_line_key(line_key);

    c.stop();
    c.start();

    while( inputfile )
    {
        char  command;
        xy    point;
        float stime;
        xy    curve_pts[4];

        inputfile >> command;

        switch( command )
        {
            case 'm':
            case 'M':
                inputfile >> point.x;
                inputfile >> point.y;
                cout << "Moving to " << point.x << ',' << point.y << endl;
                c.move_to( point );
                break;

            case 'c':
            case 'C':
                inputfile >> point.x;
                inputfile >> point.y;
                cout << "Cutting to " << point.x << ',' << point.y << endl;
                c.cut_to( point );
                break;

            case 'b':
            case 'B':
                inputfile >> curve_pts[0].x;
                inputfile >> curve_pts[0].y;
                inputfile >> curve_pts[1].x;
                inputfile >> curve_pts[1].y;
                inputfile >> curve_pts[2].x;
                inputfile >> curve_pts[2].y;
                inputfile >> curve_pts[3].x;
                inputfile >> curve_pts[3].y;
                cout<< "Bezier pts: "
                    << curve_pts[0].x << ',' << curve_pts[0].y << '\t'
                    << curve_pts[1].x << ',' << curve_pts[1].y << '\t'
                    << curve_pts[2].x << ',' << curve_pts[2].y << '\t'
                    << curve_pts[3].x << ',' << curve_pts[3].y << endl;
                c.curve_to( curve_pts[0], curve_pts[1], curve_pts[2], curve_pts[3] );
                break;

            case 's':
            case 'S':
                inputfile >> stime;
                sleep( stime );
                break;

            default:
                cout<<"Did not understand:"<<command<<endl;
                break;
        }
    }
    c.stop();
    return 0;
}
