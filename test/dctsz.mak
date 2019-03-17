.PHONY: all clean

TARGET = dctsz
INCLUDE = -I../src/parsers -I../src/picture -I../src/stream -I../src/bitbuffer -I../src/thread

HDR_DEPS1 = ../src/parsers/block.h ../src/bitbuffer/buf_bitbuf.h ../src/bitbuffer/bitbuf.h
HDR_DEPS2 = ../src/thread/doorbell.h ../src/thread/thread.h ../src/stream/stream.h
HDR_DEPS3 = ../src/picture/picdata.h
CPP_DEPS1 = ../src/parsers/block.cpp ../src/bitbuffer/buf_bitbuf.cpp ../src/bitbuffer/bitbuf.cpp
CPP_DEPS2 = ../src/thread/doorbell.cpp ../src/thread/thread.cpp ../src/stream/stream.cpp
CPP_DEPS3 = ../src/picture/picdata.cpp

HDR_DEPS = $(HDR_DEPS1) $(HDR_DEPS2) $(HDR_DEPS3)
CPP_DEPS = $(CPP_DEPS1) $(CPP_DEPS2) $(CPP_DEPS3)

all: $(TARGET)

$(TARGET): $(HDR_DEPS) $(CPP_DEPS) dctsz.cpp
	   g++ -DTEST $(INCLUDE) -g -o $@ $(CPP_DEPS) dctsz.cpp -lpthread
	   
$(TARGET): 
clean:
	rm -f *~ core *.o $(TARGET)
	