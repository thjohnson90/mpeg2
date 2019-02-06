include ../../Makefile.inc

.PHONY: all clean

FILES = bitbuf file_bitbuf buf_bitbuf
SRCS = $(addsuffix .cpp, $(FILES))
HDRS = $(addsuffix .h, $(FILES))
OBJS = $(patsubst %.cpp, %.o, $(SRCS))

all: $(OBJS) $(HDRS)

clean:
	rm -f *~ *.o core
