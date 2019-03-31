BLDDIR = build
SRCDIR = src

MAKE = make
ROOT = /home/tmjohns1/projects/mpeg2/src
TARGET = mpeg2

INCDIRS = stream bitbuffer parsers picture thread video include
INCLUDES = $(foreach dir, $(INCDIRS), -I$(ROOT)/$(dir))

SRCEXT = cpp

#CXXFLAGS = -std=c++14 -DDBG
CXXFLAGS = -std=c++14
OPT = -g -O2 -faggressive-loop-optimizations -ffast-math -mavx2
RM = rm -rf

%.o : %.cpp
	$(CXX) -c $(CFLAGS) $(OPT) $(CXXFLAGS) $(INCLUDES) -o $@ $<
