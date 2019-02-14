include Makefile.inc

.PHONY: all clean

all: $(BLDDIR)
	$(MAKE) -C $(SRCDIR) all

$(BLDDIR):
	mkdir $(BLDDIR)

clean:
	$(MAKE) clean -C $(SRCDIR)
	$(RM) $(BLDDIR) *~

