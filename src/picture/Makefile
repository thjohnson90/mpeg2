include ../../Makefile.inc

.PHONY: all clean

FILES = picdata
SRCS = $(addsuffix .cpp, $(FILES))
HDRS = $(addsuffix .h, $(FIELS))
OBJS = $(patsubst %.cpp, %.o, $(SRCS))

all: $(OBJS) $(HDRS)

clean:
	rm -f *~ *.o core
