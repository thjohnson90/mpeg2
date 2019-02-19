include ../../Makefile.inc

.PHONY: all clean

FILES = base_parser extension gop pack peshdr picture sequence slice syshdr user macroblk
SRCS = $(addsuffix .cpp, $(FILES))
HDRS = $(addsuffix .h, $(FILES))
OBJS = $(patsubst %.cpp, %.o, $(SRCS))

all: $(OBJS) $(HDRS)

clean:
	rm -f *~ *.o core
