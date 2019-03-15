.PHONY: all clean

TARGET = dct
INCLUDE = -I../src/parsers -I../src/picture -I../src/stream -I../src/bitbuffer -I../src/thread

HDR_DEPS1 = ../src/parsers/block.h ../src/bitbuffer/buf_bitbuf.h ../src/bitbuffer/bitbuf.h
HDR_DEPS2 = ../src/thread/doorbell.h ../src/thread/thread.h ../src/stream/stream.h
CPP_DEPS1 = ../src/parsers/block.cpp ../src/bitbuffer/buf_bitbuf.cpp ../src/bitbuffer/bitbuf.cpp
CPP_DEPS2 = ../src/thread/doorbell.cpp ../src/thread/thread.cpp ../src/stream/stream.cpp

HDR_DEPS = $(HDR_DEPS1) $(HDR_DEPS2)
CPP_DEPS = $(CPP_DEPS1) $(CPP_DEPS2)

all: $(TARGET)

$(TARGET): $(HDR_DEPS) $(CPP_DEPS) dct.cpp
	   g++ -DTEST $(INCLUDE) -g -o $@ $(CPP_DEPS) dct.cpp -lpthread
	   
$(TARGET): 
clean:
	rm -f *~ core *.o $(TARGET)
	