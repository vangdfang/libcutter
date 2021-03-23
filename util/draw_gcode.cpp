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
#include "keys.h"

using namespace std;

#include "gcode.hpp"

void usage(char *progname)
{
     printf("Usage: %s --mk <move keys (separated by spaces)> [-d debug_level] <device file> <gcode file>\n",
	    progname);
     cout << "\t--mk - 4 move keys, separated by spaces, like this: `--mk 12ab56cd 87ab43cd 12ab56cd 87ab43cd`" << endl;
     printf("%s\n", debug_msg.c_str());
     exit(1);
}

struct LaunchOptions {
     debug_prio debug_priority = err;
     optional<string> device_file{};
     optional<string> gcode_file{};

     optional<individual_key_t> moveKey0{};
     optional<individual_key_t> moveKey1{};
     optional<individual_key_t> moveKey2{};
     optional<individual_key_t> moveKey3{};
};

LaunchOptions parseArgs(int num_args, char * args[])
{
     LaunchOptions options{};

     // Arguments should start after the command line (args[0]).
     int i = 1;
     while (i < num_args) 
     {
          auto currentArg = string(args[i]);
          const bool hasParsedDeviceFile = options.device_file.has_value();

          if (currentArg == "-d")
          {
               // The next argument is the debug level.
               options.debug_priority = (enum debug_prio)strtol(args[i + 1], NULL, 10);
               i += 2;
               break;
          }
          else if (currentArg == "--mk")
          {
               if (num_args - i < 5) {
                    throw runtime_error("Not enough arguments; expected 4 keys.");
               }
               // The next 4 argument are the move keys
               options.moveKey0 = stoi(args[i + 1], nullptr /* idx */, 16 /* base */);
               options.moveKey1 = stoi(args[i + 2], nullptr /* idx */, 16 /* base */);
               options.moveKey2 = stoi(args[i + 3], nullptr /* idx */, 16 /* base */);
               options.moveKey3 = stoi(args[i + 4], nullptr /* idx */, 16 /* base */);
               i += 5;
               break;
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
          usage(args[0]);
     }

     // Verify that keys were set at compile time or that they were provided at runtime.
     #ifdef NO_COMPILE_TIME_KEYS
          if (!launchOptions.moveKey0 ||
               !launchOptions.moveKey1 ||
               !launchOptions.moveKey2 ||
               !launchOptions.moveKey3)
          {
               cerr << "Not all move keys were provided (did you provide 4 keys with `--mk`?)" << endl;
               cerr << endl;
               cerr << "Provided: "
                    << launchOptions.moveKey0.value_or(0ul) << " "
                    << launchOptions.moveKey1.value_or(0ul) << " "
                    << launchOptions.moveKey2.value_or(0ul) << " "
                    << launchOptions.moveKey3.value_or(0ul) << " "
                    << endl;
               cerr << endl;
               usage(args[0]);
          }

          ckey_type move_key={*launchOptions.moveKey0, *launchOptions.moveKey1, *launchOptions.moveKey2, *launchOptions.moveKey3 };
          ckey_type line_key={LINE_KEY_0, LINE_KEY_1, LINE_KEY_2, LINE_KEY_3 };
          ckey_type curve_key={CURVE_KEY_0, CURVE_KEY_1, CURVE_KEY_2, CURVE_KEY_3 };
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
