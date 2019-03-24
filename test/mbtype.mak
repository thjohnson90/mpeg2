.PHONY: all clean

TARGET = mbtype
INCLUDE = -I../src/parsers -I../src/picture -I../src/stream -I../src/bitbuffer -I../src/thread -I../src/video

HDR_DEPS1 = ../src/bitbuffer/buf_bitbuf.h ../src/bitbuffer/bitbuf.h ../src/parsers/macroblk.h
HDR_DEPS2 = ../src/thread/doorbell.h ../src/thread/thread.h ../src/stream/stream.h
HDR_DEPS3 = ../src/parsers/motvecs.h ../src/parsers/block.h ../src/picture/picdata.h ../src/video/videoproc.h
CPP_DEPS1 = ../src/bitbuffer/buf_bitbuf.cpp ../src/bitbuffer/bitbuf.cpp ../src/parsers/macroblk.cpp
CPP_DEPS2 = ../src/thread/doorbell.cpp ../src/thread/thread.cpp ../src/stream/stream.cpp
CPP_DEPS3 = ../src/parsers/motvecs.cpp ../src/parsers/block.cpp ../src/picture/picdata.cpp ../src/video/videoproc.cpp

HDR_DEPS = $(HDR_DEPS1) $(HDR_DEPS2) $(HDR_DEPS3)
CPP_DEPS = $(CPP_DEPS1) $(CPP_DEPS2) $(CPP_DEPS3)

all: $(TARGET)

$(TARGET): $(HDR_DEPS) $(CPP_DEPS) mbtype.cpp
	   g++ -DTEST $(INCLUDE) -g -o $@ $(CPP_DEPS) mbtype.cpp -lpthread
	   
$(TARGET): 
clean:
	rm -f *~ core *.o $(TARGET)
	