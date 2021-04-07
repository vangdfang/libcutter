Serial communications and a simple test application for interfacing with cutters.

Film at 11.

# Supported Cutters
- Cricut Personal - Unknown which firmware version(s)
- Cricut Expression - v2.31

# Encryption / communication keys
These are not provided with the library for various reasons. Please do not ask
for them here or elsewhere.

## Providing keys

You must provide keys at runtime by passing in a path to a key configuration file to each utility.

Key config files are a simple text file with a key's name and a key on each line, separated by whitespace. Like this (using fake keys):

```
MOVE_KEY_0  0x0123abcd
MOVE_KEY_1  0x0123abcd
MOVE_KEY_2  0x0123abcd
MOVE_KEY_3  0x0123abcd
LINE_KEY_0  0x0123abcd
LINE_KEY_1  0x0123abcd
LINE_KEY_2  0x0123abcd
LINE_KEY_3  0x0123abcd
CURVE_KEY_0  0x0123abcd
CURVE_KEY_1  0x0123abcd
CURVE_KEY_2  0x0123abcd
CURVE_KEY_3  0x0123abcd
```

Running any command that requires keys will also explain this format :).

# Developer Notes
## Code formatting
Suggested bcpp command:
bcpp -s -f 2 -i 4 -bnl -fi $in -fo $out

## Dependencies
libcutter itself does not have many dependencies outside of OS-level serial port drivers. However, some of the utilities need the following:

- libsvg - a copy is in deps folder, need to patch png_set_gray_1_2_4_to_8 to png_set_expand_gray_1_2_4_to_8 in src/svg_image.c
- libpng - needed by libsvg
- SDL2 & SDL2GFX - needed by some of the simulator tools

### Installing libsvg

If you're on a Mac (though you may need to wait for [the formula to be fixed](https://github.com/Homebrew/homebrew-core/pull/68760)):

```bash
brew install libsvg # or `brew install --build-from-source libsvg`
```

Otherwise, extract the file in the `deps/` directory and follow the instructions in `INSTALL`. You may need to apply the patch mentioned above: patch `png_set_gray_1_2_4_to_8` to `png_set_expand_gray_1_2_4_to_8` in `src/svg_image.c`.

## Building for Linux/UNIX/Mac

From this directory:

- `mkdir build`
- `cd build`
- `cmake ..`
- `make`
- Yay! Now you can run the code: `./util/some-program`

## Building for Windows 
To build for win32, you first need MinGW. On Debian install the following packages:

- mingw-w64
- mingw-w64-i686-dev

Once done, ./build-win32.sh has a sample for how to build the library and utilites.

Note, currently some of the utilities do not build on Win32 if libsvg is not present.
