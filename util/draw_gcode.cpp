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

using namespace std;

#include "gcode.hpp"

void usage(char *progname)
{
     printf("Usage: %s <device file> <gcode file> [-d debug_level]\n",
	    progname);
     printf("%s\n", debug_msg.c_str());
     exit(1);
}

struct LaunchOptions {
     debug_prio debug_priority = err;
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
     const auto launchOptions = ([&]() -> LaunchOptions
     {
          try
          {
               return parseArgs(num_args, args);
          }
          catch(const exception& e)
          {
               cerr << "Failed to parse args:" << endl;
               cerr << e.what() << endl;
               cerr << endl;
               usage(args[0]);
               exit(1);
          }
     }());

     if (!launchOptions.device_file || !launchOptions.gcode_file)
     {
          cerr << "Please provide a device file and GCode file" << endl;
          cerr << endl;
          cerr << "Provided device: " << launchOptions.device_file.value_or("(missing)") << endl;
          cerr << "Provided GCode: " << launchOptions.gcode_file.value_or("(missing)") << endl;
          cerr << endl;
          usage(args[0]);
     }

     KeyConfigParser key_config;

     auto moveKeys = key_config.moveKeys();
     auto lineKeys = key_config.lineKeys();
     auto curveKeys = key_config.curveKeys();
     ckey_type move_key = { moveKeys.key0, moveKeys.key1, moveKeys.key2, moveKeys.key3 };
     ckey_type line_key = { lineKeys.key0, lineKeys.key1, lineKeys.key2, lineKeys.key3 };
     ckey_type curve_key = { curveKeys.key0, curveKeys.key1, curveKeys.key2, curveKeys.key3 };

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
