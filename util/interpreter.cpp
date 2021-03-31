#include <string>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <unistd.h>
using namespace std;

#include "keys.h"
#include "device_c.hpp"
#include "KeyConfigParser.hpp"

int main( int numArgs, char * args[] )
{
    #ifdef NO_COMPILE_TIME_KEYS
        if( numArgs != 4 )
        {
            cout<<"Usage: "<<args[0]<<" <device file> <input file> <key config file>"<<endl;
            cout << endl;
            cout << "\t<device file> - file of the cutter. Looks like '/dev/serial/port' or '/dev/cu.usbserial-10'" << endl;
            cout << endl;
            cout << "\t<input file> - file to interpret. Formatted:" << endl;
            cout << endl;
            cout << "\t\tMove to: m <x> <y>" << endl;
            cout << "\t\tCut to: c <x> <y>" << endl;
            cout << "\t\tCurve: b <x1> <y1> <x2> <y2> <x3> <y3> <x4> <y4>" << endl;
            cout << "\t\tSleep: t <millis>" << endl;
            cout << endl;
            cout << "\t<key config file> - key configuration file, which contains cutting keys. For example (with fake keys):" << endl;
            cout << endl;
            cout << "\t\tMOVE_KEY_0  0x0123abcd" << endl;
            cout << "\t\tMOVE_KEY_1  0x0123abcd" << endl;
            cout << "\t\tMOVE_KEY_2  0x0123abcd" << endl;
            cout << "\t\tMOVE_KEY_3  0x0123abcd" << endl;
            cout << "\t\tLINE_KEY_0  0x0123abcd" << endl;
            cout << "\t\tLINE_KEY_1  0x0123abcd" << endl;
            cout << "\t\tLINE_KEY_2  0x0123abcd" << endl;
            cout << "\t\tLINE_KEY_3  0x0123abcd" << endl;
            cout << "\t\tCURVE_KEY_0  0x0123abcd" << endl;
            cout << "\t\tCURVE_KEY_1  0x0123abcd" << endl;
            cout << "\t\tCURVE_KEY_2  0x0123abcd" << endl;
            cout << "\t\tCURVE_KEY_3  0x0123abcd" << endl;
            exit(1);
        }
    #else
        if( numArgs != 3 )
        {
            cout<<"Usage: "<<args[0]<<" <device file> <input file>"<<endl;
            cout << endl;
            cout << "\t<device file> - file of the cutter. Looks like '/dev/serial/port' or '/dev/cu.usbserial-10'" << endl;
            cout << endl;
            cout << "\t<input file> - file to interpret. Formatted:" << endl;
            cout << endl;
            cout << "\t\tMove to: m <x> <y>" << endl;
            cout << "\t\tCut to: c <x> <y>" << endl;
            cout << "\t\tCurve: b <x1> <y1> <x2> <y2> <x3> <y3> <x4> <y4>" << endl;
            cout << "\t\tSleep: t <millis>" << endl;
            exit(1);
        }
    #endif

    ifstream inputfile(args[2] );
    if(!inputfile )
    {
        cout<<"Error opening file"<<endl;
        exit(2);
    }

    Device::C c(args[1]);

    #ifdef NO_COMPILE_TIME_KEYS
        KeyConfigParser keyConfig(args[3]);

        auto moveKeys = keyConfig.moveKeys();
        auto lineKeys = keyConfig.lineKeys();
        auto curveKeys = keyConfig.curveKeys();
        ckey_type move_key = { moveKeys.key0, moveKeys.key1, moveKeys.key2, moveKeys.key3 };
        ckey_type line_key = { lineKeys.key0, lineKeys.key1, lineKeys.key2, lineKeys.key3 };
        ckey_type curve_key = { curveKeys.key0, curveKeys.key1, curveKeys.key2, curveKeys.key3 };
    #else
        ckey_type move_key={MOVE_KEY_0, MOVE_KEY_1, MOVE_KEY_2, MOVE_KEY_3 };
        ckey_type line_key={LINE_KEY_0, LINE_KEY_1, LINE_KEY_2, LINE_KEY_3 };
        ckey_type curve_key={CURVE_KEY_0, CURVE_KEY_1, CURVE_KEY_2, CURVE_KEY_3 };
    #endif

    c.set_move_key(move_key);
    c.set_line_key(line_key);
    c.set_curve_key(curve_key);

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
