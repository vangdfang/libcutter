#include <string>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <unistd.h>
using namespace std;

#include "device_sim.hpp"

int main( int numArgs, char * args[] )
{
    if( numArgs != 3 )
    {
        cout<<"Usage: "<<args[0]<<" <device file> <input file>"<<endl;
        cout << endl;
        cout << "\t<device file> - serial port file of the cutter. Looks like:" << endl;
        cout << "\t\t/dev/ttyUSBx" << endl;
        cout << "\t\t/dev/cu.usbserial-10" << endl;
        cout << "\t\t/dev/serial/port" << endl;
        cout << endl;
        cout << "\t<input file> - file to interpret. Formatted:" << endl;
        cout << endl;
        cout << "\t\tMove to: m <x> <y>" << endl;
        cout << "\t\tCut to: c <x> <y>" << endl;
        cout << "\t\tCurve: b <x1> <y1> <x2> <y2> <x3> <y3> <x4> <y4>" << endl;
        cout << "\t\tSleep: t <millis>" << endl;
        exit(1);
    }

    ifstream inputfile(args[2] );
    if(!inputfile )
    {
        cout<<"Error opening file"<<endl;
        exit(2);
    }

    Device::CV_sim c(args[1]);

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

    sleep(1);
    c.stop();
    return 0;
}
