#include <sys/time.h>
#include <device_c.hpp>
#include <iostream>
#include <signal.h>
#include <stdlib.h>
#include <math.h>
using namespace std;

#include "KeyConfigParser.hpp"

int main(int argc, char *argv[])
{
    if( argc != 2 )
    {
        cout<<"Usage: "<<argv[0]<<" <device file>"<<endl;
        cout << endl;
        cout << "\t<device file> - serial port file of the cutter. Looks like:" << endl;
        cout << "\t\t/dev/ttyUSBx" << endl;
        cout << "\t\t/dev/cu.usbserial-10" << endl;
        cout << "\t\t/dev/serial/port" << endl;
        exit(1);
    }

    Device::C cutter(argv[1]);

    cout <<"Device Found:" << cutter.device_name() << std::endl;
    cout <<"Mat Size:" << cutter.get_dimensions().x << "x" << cutter.get_dimensions().y << std::endl;

    return 0;
}
