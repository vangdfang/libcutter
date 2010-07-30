CXXFLAGS=-DSERIAL_PORT_DEBUG_MODE

all: test_serial

test_serial: test_serial.cpp serial_port.cpp

clean:
	rm -rf test_serial
