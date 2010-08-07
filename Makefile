CXXFLAGS=-DSERIAL_PORT_DEBUG_MODE -I./pub

all: .depend test_serial jsdrive jsdrive_cv

.depend:
	fastdep --remakedeptarget=.depend *.c *.cpp > .depend

include .depend

#nclude Makefile.deps

jsdrive: serial_port.o jsdrive.o device_c.o device.o btea.o
	g++ -o $@ $+ -lpthread

jsdrive_cv: device.o device_cv_sim.o jsdrive_cv.o
	g++ -o $@ $+ -lpthread -lcv -lhighgui

test_serial: test_serial.o serial_port.o
	g++ -o $@ $+

clean:
	rm -rf test_serial jsdrive jsdrive_cv *.o


