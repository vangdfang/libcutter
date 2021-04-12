/*
 * libcutter - xy cutter control library
 * Copyright (c) 2010 - libcutter Developers
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Should you need to contact us, the author, you can do so at
 * http://github.com/vangdfang/libcutter
 */
#include <stdint.h>
#include <unistd.h>
#include <iostream>
#include "device_sim.hpp"
#include "types.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#define DPI_X 100
#define DPI_Y 100

#define DEFAULT_SIZE_X 12
#define DEFAULT_SIZE_Y 12

#define WIDTH  ( ( DPI_X ) * ( DEFAULT_SIZE_X ) )
#define HEIGHT ( ( DPI_Y ) * ( DEFAULT_SIZE_Y ) )

#define FORMAT SDL_PIXELFORMAT_ARGB8888

#define DEPTH 32

namespace Device
{

    CV_sim::CV_sim()
    {
        running            = false;
        screen             = NULL;
        renderer           = NULL;
        current_position.x = 0;
        current_position.y = 0;
        tool_width         = 1;
    }

    CV_sim::CV_sim( const std::string filename )
    {
        output_filename    = filename;
        running            = false;
        screen             = NULL;
        renderer           = NULL;
        current_position.x = 0;
        current_position.y = 0;
        tool_width         = 1;
    }

    bool CV_sim::move_to(const xy & aPoint )
    {
        if( !running )
        {
            return false;
        }

        current_position = convert_to_internal( aPoint );
        return true;
    }

    bool CV_sim::cut_to(const xy & aPoint )
    {
        xy next_position;
        xy external_cur_posn = convert_to_external( current_position );

        double distance = sqrt( ( external_cur_posn.x - aPoint.x ) * ( external_cur_posn.x - aPoint.x ) +
            ( external_cur_posn.y - aPoint.y ) * ( external_cur_posn.y - aPoint.y ) );

        if( !running )
        {
            return false;
        }

        next_position = convert_to_internal( aPoint );

        if( renderer != NULL )
        {
            lineRGBA( renderer, current_position.x, current_position.y, next_position.x, next_position.y, 250, 50, 50, 200 );
            SDL_RenderPresent( renderer );
            usleep( 100000 * distance );
        }

        current_position = next_position;
        return true;
    }

    bool CV_sim::curve_to(const xy & p0, const xy & p1, const xy & p2, const xy & p3 )
    {
        #define A 0
        #define B 1
        #define C 2
        #define NUM_FACT 3
        #define X 0
        #define Y 1
        #define NUM_DIM 2

        #define NUM_SECTIONS_PER_CURVE 20

        double coeff[ NUM_FACT ][ NUM_DIM ];
        xy iter;
        double t;

        if( !running )
        {
            return false;
        }

        coeff[C][X] = 3 * ( p1.x - p0.x );
        coeff[B][X] = 3 * ( p2.x - p1.x ) - coeff[C][X];
        coeff[A][X] = ( p3.x - p0.x ) - coeff[C][X] - coeff[B][X];

        coeff[C][Y] = 3 * ( p1.y - p0.y );
        coeff[B][Y] = 3 * ( p2.y - p1.y ) - coeff[C][Y];
        coeff[A][Y] = ( p3.y - p0.y ) - coeff[C][Y] - coeff[B][Y];

        move_to( p0 );
        for( int i = 1; i <= NUM_SECTIONS_PER_CURVE; ++i )
        {
            t = (double)i / (double)NUM_SECTIONS_PER_CURVE;
            iter.x = coeff[A][X] * t * t * t + coeff[B][X] * t * t + coeff[C][X] * t + p0.x;
            iter.y = coeff[A][Y] * t * t * t + coeff[B][Y] * t * t + coeff[C][Y] * t + p0.y;
            cut_to( iter );
        }

        return true;
        #undef A
        #undef B
        #undef C
        #undef X
        #undef Y
        #undef NUM_FACT
        #undef NUM_DIM
        #undef NUM_SECTIONS_PER_CURVE
    }

    bool CV_sim::start()
    {
        if( renderer == NULL )
        {
            SDL_Init( SDL_INIT_VIDEO );
            screen = SDL_CreateWindow( "Simulated Cutter", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, 0 );
            if( screen == NULL ) {
                std::cerr << "Unable to create SDL2 window!" << std::endl;
                return false;
            }
            renderer = SDL_CreateRenderer( screen, -1, SDL_RENDERER_TARGETTEXTURE | SDL_RENDERER_SOFTWARE );
            if( renderer == NULL ) {
                std::cerr << "Unable to create SDL2 renderer!" << std::endl;
                SDL_Quit();
                return false;
            }
        }
        running = true;
        return true;
    }

    bool CV_sim::stop()
    {
        int retn;

        if( renderer != NULL && output_filename.size() > 4 )
        {
            SDL_Surface * image = SDL_CreateRGBSurfaceWithFormat( 0, WIDTH, HEIGHT, DEPTH, SDL_PIXELFORMAT_ARGB8888 );
            SDL_RenderReadPixels( renderer, NULL, SDL_PIXELFORMAT_ARGB8888, image->pixels, image->pitch );
            retn = SDL_SaveBMP( image, output_filename.c_str() );
            SDL_FreeSurface( image );
            SDL_Quit();
        }
        running = false;
        return retn == 0;
    }

    xy CV_sim::convert_to_internal( const xy & input )
    {
        xy buf;

        buf.x = input.x * DPI_X;
        buf.y = input.y * DPI_Y;

        return buf;
    }

    xy CV_sim::convert_to_external( const xy & input )
    {
        xy buf;

        buf.x = input.x / DPI_X;
        buf.y = input.y / DPI_Y;

        return buf;
    }

    xy CV_sim::get_dimensions( void )
    {
        xy buf;
        buf.x = DEFAULT_SIZE_X;
        buf.y = DEFAULT_SIZE_Y;
        return buf;
    }

    bool CV_sim::set_tool_width( const float temp_tool_width )
    {
        if( temp_tool_width > 0 )
        {
            tool_width = fabs( temp_tool_width ) * DPI_X * DPI_Y / sqrt( DPI_X * DPI_Y ) + .5;
            if( tool_width < 1 )
            {
                tool_width = 1;
            }
            return true;
        }
        return false;
    }

    SDL_Surface * CV_sim::get_image()
    {
        // Create a Surface from the current window
        SDL_Surface * new_image = SDL_CreateRGBSurfaceWithFormat( 0, WIDTH, HEIGHT, DEPTH, 0 );
        SDL_RenderReadPixels( renderer, NULL, FORMAT, new_image->pixels, new_image->pitch );

        // Copy the Surface into a Texture for drawing on it
        SDL_Texture * texture = SDL_CreateTextureFromSurface( renderer, new_image );
        
        // Draw a crosshair
        SDL_SetRenderTarget( renderer, texture );
        ellipseRGBA( renderer, current_position.x, current_position.y, 10, 10, 50, 250, 50, 200 );
        aalineRGBA( renderer, current_position.x + 5, current_position.y + 5, current_position.x - 5, current_position.y - 5, 250, 50, 50, 200 );
        aalineRGBA( renderer, current_position.x + 5, current_position.y - 5, current_position.x - 5, current_position.y + 5, 250, 50, 50, 200 );

        // Copy the Texture back to the Surface with the added crosshair
        SDL_Rect rect = { 0, 0, WIDTH, HEIGHT };
        SDL_RenderReadPixels( renderer, &rect, FORMAT, new_image->pixels, new_image->pitch );
        SDL_SetRenderTarget( renderer, NULL );
        SDL_DestroyTexture( texture );

        return new_image;
    }

}                                /* end namespace*/
