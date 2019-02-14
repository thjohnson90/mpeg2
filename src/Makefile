#
# makefile
#
include ../Makefile.inc

SRCS = mpeg2.cpp
SUBDIRS := $(wildcard */.)

OBJS = $(shell find . -name '*.o')

all: $(SUBDIRS) $(TARGET)

$(SUBDIRS):
	$(MAKE) -C $@

$(TARGET): $(SRCS) $(OBJS)
	@echo -------------------- Linking Main ----------------
	$(CXX) $(OPT) $(INCLUDES) -o $@ $? $(OBJS) -lpthread
	cp $(ROOT)/$(TARGET) $(ROOT)/../$(BLDDIR)/.


clean:
	for dir in $(SUBDIRS); do \
		$(MAKE) clean -C $$dir; \
	done
	$(RM) *~ $(TARGET)

.PHONY: all clean install $(SUBDIRS)
