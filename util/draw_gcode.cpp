//This file adapted from http://sites.google.com/site/drbobbobswebsite/cricut-gcode-interpreter

#include <cmath>
#include <vector>
#include <string>
#include <optional>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include "device_c.hpp"
#include "KeyConfigParser.hpp"
#include "keys.h"

using namespace std;

#include "gcode.hpp"

void usage(char *progname)
{
     printf("Usage: %s <device file> <gcode file> -f <key config file> [-d debug_level]\n",
	    progname);
     // TODO besto
     cout << "\t--mk - 4 move keys, separated by spaces, like this: `--mk 12ab56cd 87ab43cd 12ab56cd 87ab43cd`" << endl;
     cout << "\t--lk - 4 line keys, separated by spaces, like this: `--lk 12ab56cd 87ab43cd 12ab56cd 87ab43cd`" << endl;
     cout << "\t--ck - 4 curve keys, separated by spaces, like this: `--ck 12ab56cd 87ab43cd 12ab56cd 87ab43cd`" << endl;
     printf("%s\n", debug_msg.c_str());
     exit(1);
}

struct LaunchOptions {
     debug_prio debug_priority = err;
     optional<KeyConfigParser> key_config;
     optional<string> device_file{};
     optional<string> gcode_file{};
};

LaunchOptions parseArgs(int num_args, char * args[])
{
     LaunchOptions options{};

     // Arguments should start after the command line (args[0]).
     int i = 1;
     while (i < num_args) 
     {
          const auto currentArg = string(args[i]);
          const bool hasParsedDeviceFile = options.device_file.has_value();

          if (currentArg == "-d")
          {
               // The next argument is the debug level.
               options.debug_priority = (enum debug_prio)strtol(args[i + 1], NULL, 10);
               i += 2;
               continue;
          }
          else if (currentArg == "-f")
          {
               // The next argument is the config file.
               options.key_config = KeyConfigParser(args[i + 1]);
               i += 2;
               continue;
          }
          else if (!hasParsedDeviceFile)
          {
               // First bare argument should be device file.
               options.device_file = currentArg;
          }
          else if (hasParsedDeviceFile)
          {
               // Second bare argument should be gcode file.
               options.gcode_file = currentArg;
          }

          // Parse the next arg.
          i++;
     }

     return options;
}

int main( int num_args, char * args[] )
{
     const auto launchOptions = parseArgs(num_args, args);
     if (!launchOptions.device_file || !launchOptions.gcode_file)
     {
          #ifdef NO_COMPILE_TIME_KEYS
               cerr << "Please provide a device file, GCode file, and key configuration file" << endl;
          #else
               cerr << "Please provide a device file and GCode file" << endl;
          #endif
          cerr << endl;
          cerr << "Provided device: " << launchOptions.device_file.value_or("(missing)") << endl;
          cerr << "Provided GCode: " << launchOptions.gcode_file.value_or("(missing)") << endl;
          #ifdef NO_COMPILE_TIME_KEYS
               cerr << "Provided key config: " << (launchOptions.key_config ? "Provided" : "(missing)") << endl;
          #endif;
          cerr << endl;
          usage(args[0]);
     }

     // Verify that keys were set at compile time or that they were provided at runtime.
     #ifdef NO_COMPILE_TIME_KEYS
          if (!launchOptions.key_config)
          {
               cerr << "Please provide a key configuration file (did you add the `-f` flag?)" << endl;
               cerr << endl;
               usage(args[0]);
          }

          auto moveKeys = launchOptions.key_config->moveKeys();
          auto lineKeys = launchOptions.key_config->lineKeys();
          auto curveKeys = launchOptions.key_config->curveKeys();
          ckey_type move_key = { moveKeys.key0, moveKeys.key1, moveKeys.key2, moveKeys.key3 };
          ckey_type line_key = { lineKeys.key0, lineKeys.key1, lineKeys.key2, lineKeys.key3 };
          ckey_type curve_key = { curveKeys.key0, curveKeys.key1, curveKeys.key2, curveKeys.key3 };
     #else
          ckey_type move_key={MOVE_KEY_0, MOVE_KEY_1, MOVE_KEY_2, MOVE_KEY_3 };
          ckey_type line_key={LINE_KEY_0, LINE_KEY_1, LINE_KEY_2, LINE_KEY_3 };
          ckey_type curve_key={CURVE_KEY_0, CURVE_KEY_1, CURVE_KEY_2, CURVE_KEY_3 };
     #endif

     Device::C cutter(*launchOptions.device_file);
     gcode parser(*launchOptions.gcode_file, cutter);
     gcode_base::set_debug(launchOptions.debug_priority);

     cutter.stop();
     cutter.start();

     cutter.set_move_key(move_key);
     cutter.set_line_key(line_key);
     cutter.set_curve_key(curve_key);

     try
     {
	       parser.parse_file();
     }
     catch(...)
     {
	  printf("Unhandled exception");
     }

     cutter.stop();

     return 0;
}
