#include <sys/time.h>
#include <device_c.hpp>
#include <iostream>
#include <signal.h>
#include <stdlib.h>
using namespace std;

#include "KeyConfigParser.hpp"

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


int main(int argc, char *argv[])
{
    xy startpt = { 1.5, 2 };
    xy endpt   = { 4.5, 8 };
    uint64_t     timer;

    signal(SIGINT, clean_up);

    if( argc != 3 )
    {
        cout<<"Usage: "<<argv[0]<<" <device file>"<<endl;
        cout << endl;
        cout << "\t<device file> - file of the cutter. Looks like '/dev/serial/port' or '/dev/cu.usbserial-10'" << endl;
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

    Device::C cutter(argv[1]);

    KeyConfigParser keyConfig(argv[2]);

    auto moveKeys = keyConfig.moveKeys();
    auto lineKeys = keyConfig.lineKeys();
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
        cutter.cut_to( startpt );
        cutter.cut_to( endpt   );
    }
    cutter.cut_to( endpt   );
    cout << "Took " << ( getCurTime() - timer ) / 1000000.0 / ( NUM_RUNS * 2 + 1 ) << "per path. Multiply by two to get full travel time" << endl;

    cutter.stop();

    return 0;
}
