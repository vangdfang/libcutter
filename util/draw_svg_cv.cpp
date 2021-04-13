#include <iostream>
#include <cstring>
#include <cmath>
#include <svg.h>
#include <unistd.h>

#include "device_sim.hpp"
#include "svg.hpp"

int main(int numArgs, char * args[] )
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
        exit(1);
    }

    if( numArgs != 2 )
    {
        std::cout<<"Usage: "<<args[0]<<" svgfile.svg"<<std::endl;
        return 4;
    }

    Device::CV_sim c( "none" );
    c.stop();
    c.start();

    render_svg( args[1], c );

    sleep(1);
    c.stop();
}
