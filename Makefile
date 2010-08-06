CXXFLAGS=-DSERIAL_PORT_DEBUG_MODE -I./pub

all: test_serial jsdrive

jsdrive: serial_port.o jsdrive.o device_c.o device.o btea.o
	g++ -o jsdrive serial_port.o jsdrive.o device_c.o device.o btea.o -lpthread

test_serial: test_serial.cpp serial_port.cpp

clean:
	rm -rf test_serial jsdrive *.o
