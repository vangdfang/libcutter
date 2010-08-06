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
 * Should you need to contact me, the author, you can do so either by
 * e-mail - mail your message to <vojtech@suse.cz>, or by paper mail:
 * Vojtech Pavlik, Ucitelska 1576, Prague 8, 182 00 Czech Republic
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
#include <pthread.h>

#include <linux/joystick.h>
#include <iostream>
using namespace std;

#include "device_c.hpp"

#define NAME_LENGTH 128

static xy pt={3,4};
static bool tool_down = false;
static char cutter_device[1000];
static bool running = true;

#include "keys.h"

void * thread( void * ptr )
{
xy startpt={3,4};
Device::C c( cutter_device );

ckey_type move_key={MOVE_KEY_0, MOVE_KEY_1, MOVE_KEY_2, MOVE_KEY_3};
c.set_move_key(move_key);

ckey_type line_key={LINE_KEY_0, LINE_KEY_1, LINE_KEY_2, LINE_KEY_3 };
c.set_line_key(line_key);

c.stop();
c.start();

c.move_to(startpt);

while( running )
	{
	if( tool_down )
		{
		cout<<"Cutting to "<<pt.x<<" "<<pt.y<<endl;
		c.cut_to(pt);
		}
	else
		{
		cout<<"Moving  to "<<pt.x<<" "<<pt.y<<endl;
		c.move_to(pt);
		}
	}
}


int main (int argc, char **argv)
{
	int fd;
	unsigned char axes = 2;
	unsigned char buttons = 2;
	int version = 0x000800;
	char name[NAME_LENGTH] = "Unknown";

	if (argc != 3)
		{
		puts("Usage: jsdrive <joydevice> <cutterdevice>");
		exit(1);
		}

	strcpy( cutter_device, argv[2] );


	if ((fd = open(argv[1], O_RDONLY)) < 0)
		{
		perror("jsdrive");
		exit(3);
		}

//	fcntl(fd, F_SETFL, O_NONBLOCK);


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
		int *button;
		int i;
		struct js_event js;
		int *oldaxis;
		int *oldbutton;

		axis      = (int*)calloc(axes,    sizeof(int)  );
		oldaxis   = (int*)calloc(axes,    sizeof(int)  );
		button    = (int*)calloc(buttons, sizeof(char) );
		oldbutton = (int*)calloc(buttons, sizeof(char) );

		pthread_t tid;
		pthread_create( &tid, NULL, thread, NULL );

		while (1)
			{
			struct js_event old_js = js;
			if (read(fd, &js, sizeof(struct js_event)) != sizeof(struct js_event))
				{
				perror("\njsdrive: error reading");
				exit (1);
				}

			old_js = js;
			switch(js.type & ~JS_EVENT_INIT)
				{
				case JS_EVENT_BUTTON:
					if( js.number == 0 )
						{
						tool_down = js.value;
						}
					button[js.number] = js.value;
					break;
				case JS_EVENT_AXIS:
					axis[js.number] = js.value;
					break;
				}

			printf("\r");

			pt.x = (float)(((int)-axis[0])+32767) * 6.0 / 65535;
			pt.y = (float)(((int) axis[1])+32767) * 6.0 / 65535 + 3;
			cout<<"moving to:"<<pt.x<<' '<<pt.y<<endl;

			fflush(stdout);
		}
	}

	return -1;
}

/*
	c.stop();
	sleep(1);
	c.start();
	sleep(1);
	c.move(pt);
	sleep(3);
*/
