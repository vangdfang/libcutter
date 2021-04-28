#include <iostream>
#include <cstring>
#include <cmath>
#include <svg.h>
#include <unistd.h>

#include "device_c.hpp"
#include "KeyConfigParser.hpp"
#include "svg.hpp"

int main(int numArgs, char * args[] )
{
    if( numArgs != 3 )
    {
        std::cout<<"Usage: "<<args[0]<<" <svg file> <device file>"<<std::endl;
        std::cout << std::endl;
        std::cout << "\t<svg file> - SVG file to interpret ('foo.svg')" << std::endl;
        std::cout << std::endl;
        std::cout << "\t<device file> - file of the cutter. Looks like '/dev/serial/port' or '/dev/cu.usbserial-10 or '/dev/ttyUSBx''" << std::endl;
        exit(1);
    }

    Device::C c( args[2] );
    std::cout <<"Device Found:" << c.device_name() << std::endl;
    std::cout <<"Mat Size:" << c.get_dimensions().x << "x" << c.get_dimensions().y << std::endl;
    c.stop();
    c.start();

    KeyConfigParser keyConfig;

    auto moveKeys = keyConfig.moveKeys();
    auto lineKeys = keyConfig.lineKeys();
    auto curveKeys = keyConfig.curveKeys();
    ckey_type move_key = { moveKeys.key0, moveKeys.key1, moveKeys.key2, moveKeys.key3 };
    ckey_type line_key = { lineKeys.key0, lineKeys.key1, lineKeys.key2, lineKeys.key3 };
    ckey_type curve_key = { curveKeys.key0, curveKeys.key1, curveKeys.key2, curveKeys.key3 };

    c.set_move_key(move_key);
    c.set_line_key(line_key);
    c.set_curve_key(curve_key);

    render_svg( args[1], c );

    sleep(1);
    c.stop();
}
