/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * Should you need to contact us, the author, you can do so either at
 * http://github.com/vangdfang/libcutter, or by paper mail:
 *
 * libcutter Developers @ Cowtown Computer Congress
 * 3101 Mercier Street #404, Kansas City, MO 64111
 *
 * This file was forked from jstest.c, by Vojtech Pavlik: vojtech@suse.cz
 */

#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>

#include <linux/joystick.h>
#include <iostream>
using namespace std;

#include "device_c.hpp"

#define NAME_LENGTH 128

static char cutter_device[1000];
struct run_data
{
    bool running;
    xy pt;
    bool tool_down;
    pthread_mutex_t mutex;
};
static bool should_exit;

#include "keys.h"

void catch_sigint(int)
{
    cerr << "Shutting down..." << endl;
    should_exit = true;
}


void * thread( void * ptr )
{
    run_data *status = (run_data *)ptr;
    xy startpt={3,6};
    Device::C c( cutter_device );

    ckey_type move_key={MOVE_KEY_0, MOVE_KEY_1, MOVE_KEY_2, MOVE_KEY_3};
    c.set_move_key(move_key);

    ckey_type line_key={LINE_KEY_0, LINE_KEY_1, LINE_KEY_2, LINE_KEY_3 };
    c.set_line_key(line_key);

    if( !c.is_open() )
    {
        pthread_mutex_lock(&status->mutex);
        status->running=false;
        pthread_mutex_unlock(&status->mutex);
        pthread_exit(NULL);
    }

    c.stop();
    c.start();

    c.move_to(startpt);
    xy temp;

    pthread_mutex_lock(&status->mutex);
    while( status->running )
    {
        temp.x = status->pt.x;
        temp.y = status->pt.y;
        if( status->tool_down )
        {
            cout<<"Cutting to "<<temp.x<<" "<<temp.y<<endl;
            pthread_mutex_unlock(&status->mutex);
            c.cut_to(temp);
            pthread_mutex_lock(&status->mutex);
        }
        else
        {
            cout<<"Moving  to "<<temp.x<<" "<<temp.y<<endl;
            pthread_mutex_unlock(&status->mutex);
            c.move_to(temp);
            pthread_mutex_lock(&status->mutex);
        }
    }
    pthread_mutex_unlock(&status->mutex);
    c.stop();
    pthread_exit(NULL);
}


int main (int argc, char **argv)
{
    int fd;
    unsigned char axes = 2;
    unsigned char buttons = 2;
    int version = 0x000800;
    char name[NAME_LENGTH] = "Unknown";
    run_data status;
    status.running = true;
    status.pt.x = 3;
    status.pt.y = 6;
    status.tool_down=false;
    pthread_mutex_init(&status.mutex, NULL);

    if (argc != 3)
    {
        puts("Usage: jsdrive <joydevice> <cutterdevice>");
        exit(1);
    }

    strcpy( cutter_device, argv[2] );
    signal( SIGINT, catch_sigint );

    if ((fd = open(argv[1], O_RDONLY | O_NONBLOCK)) < 0)
    {
        perror("jsdrive");
        exit(3);
    }

    ioctl(fd, JSIOCGVERSION, &version);
    ioctl(fd, JSIOCGAXES, &axes);
    ioctl(fd, JSIOCGBUTTONS, &buttons);
    ioctl(fd, JSIOCGNAME(NAME_LENGTH), name);

    printf("Joystick (%s) has %d axes and %d buttons. Driver version is %d.%d.%d.\n",
        name, axes, buttons, version >> 16, (version >> 8) & 0xff, version & 0xff);
    printf("Testing ... (interrupt to exit)\n");

    if (argc == 3 )
    {
        int *axis;
        char *button;
        struct js_event js;

        axis      = (int *)calloc(axes,     sizeof(int)  );
        button    = (char *)calloc(buttons, sizeof(char) );

        pthread_t tid;
        pthread_create( &tid, NULL, thread, &status );

        pthread_mutex_lock(&status.mutex);
        while (status.running)
        {
            if( should_exit )
            {
                status.running=false;
            }
            pthread_mutex_unlock(&status.mutex);
            struct js_event old_js = js;
            if ( read( fd, &js, sizeof( struct js_event ) ) != sizeof( struct js_event )
                && ( EAGAIN != errno && EWOULDBLOCK != errno ) )
            {
                perror("\njsdrive: error reading");
                should_exit=true;
                continue;
            }
            else if ( EAGAIN == errno || EWOULDBLOCK == errno )
            {
                usleep(1000);
            }

            old_js = js;
            switch(js.type & ~JS_EVENT_INIT)
            {
                case JS_EVENT_BUTTON:
                    if( js.number == 0 )
                    {
                        pthread_mutex_lock(&status.mutex);
                        status.tool_down = js.value;
                        pthread_mutex_unlock(&status.mutex);
                    }
                    button[js.number] = js.value;
                    break;
                case JS_EVENT_AXIS:
                    axis[js.number] = js.value;
                    break;
            }

            printf("\r");

            pthread_mutex_lock(&status.mutex);
            status.pt.x += (float)((int)-axis[0]) * 1.0 / 65535 / 100;
            status.pt.y += (float)((int) axis[1]) * 1.0 / 65535 / 100;

			if( status.pt.x > 5.5  ) status.pt.x = 5.5;
			if( status.pt.y > 11.5 ) status.pt.y = 11.5;
			if( status.pt.x < .5   ) status.pt.x = .5;
			if( status.pt.y < .5   ) status.pt.y = .5;

            cout<<"moving to:"<<status.pt.x<<' '<<status.pt.y<<endl;
            pthread_mutex_unlock( &status.mutex );

            fflush(stdout);
            pthread_mutex_lock(&status.mutex);
        }
        pthread_mutex_unlock(&status.mutex);
        free(axis);
        free(button);
        pthread_join( tid, NULL );
    }

    return -1;
}
