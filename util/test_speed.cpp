#include <sys/time.h>
#include <device_c.hpp>
#include <iostream>
#include <signal.h>
#include <stdlib.h>
#include <math.h>
using namespace std;

#include "ConfigParser.hpp"

void clean_up(int signal)
{
    exit(signal);
}


uint64_t getCurTime( void )
{
    timeval tv;
    gettimeofday( &tv, NULL );
    return (uint64_t)tv.tv_sec * 1000000 + (uint64_t)tv.tv_usec ;
}

double sqr( double x )
{
return x*x;
}

double dist( const xy & startpt, const xy & endpt )
{
return sqrt( sqr(endpt.x - startpt.x) + sqr(endpt.y - startpt.y) );
}

int main(int argc, char *argv[])
{
    const xy endpt(0.5, 2);
    const xy startpt(5.5, 8);
    uint64_t timer;

    signal(SIGINT, clean_up);

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

    ConfigParser Config;

    Device::C cutter;
    cutter.set_serial_debug(Config.serialDebug());
    cutter.init(argv[1]);

    cout <<"Device Found:" << cutter.device_name() << std::endl;
    cout <<"Mat Size:" << cutter.get_dimensions().x << "x" << cutter.get_dimensions().y << std::endl;


    auto moveKeys = Config.moveKeys();
    auto lineKeys = Config.lineKeys();
    ckey_type move_key = { moveKeys.key0, moveKeys.key1, moveKeys.key2, moveKeys.key3 };
    ckey_type line_key = { lineKeys.key0, lineKeys.key1, lineKeys.key2, lineKeys.key3 };

    cutter.set_move_key(move_key);
    cutter.set_line_key(line_key);

    cutter.stop();
    cutter.start();

    cutter.move_to( startpt );
    timer = getCurTime();
    #define NUM_RUNS 5
    for( int i = 0; i < NUM_RUNS; ++i )
    {
        cutter.cut_to( endpt );
        cutter.cut_to( startpt );
    }
    cutter.move_to( endpt );
    timer = getCurTime() - timer;
    double seconds = timer / 1000000.0;
    cout << "Took: " << seconds << " seconds" << endl;
    cout << "Took: " << seconds / ( NUM_RUNS * 2 ) << " seconds per path" << endl;
    cout << "Speed:" << dist( startpt, endpt ) * NUM_RUNS * 2 / seconds << " inches per second" << endl;

    cutter.stop();

    return 0;
}
