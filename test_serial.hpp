#ifndef TEST_SERIAL_HPP
#define TEST_SERIAL_HPP

#include "serial_port.hpp"
#include <vector>

int main(int argc, char* argv[]);

void clean_up(int signal);

static std::vector<serial_port*> ports;

#endif
