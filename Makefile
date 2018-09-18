
prog=rdabl

.PHONY: default
default: all

srcdir=src
bindir=bin
incdir=inc
libdir=lib

cc=LD_LIBRARY_PATH=$(libdir) gcc
cflags=-I$(incdir) -Wall -g

txtdir=txt
prefix=/usr/local

$(txtdir)/sample_dic: $(txtdir)/cmudic
	chmod +x $(srcdir)/parsecmu
	cat $< | $(srcdir)/parsecmu > $@
lib = $(libdir)/lib$(prog).so
libsrc=$(srcdir)/table.c $(srcdir)/scan.c
exe=$(bindir)/$(prog)
tstexe=$(bindir)/test$(prog)
tstsrc=$(srcdir)/*test.c $(srcdir)/CuTest.c

all: $(exe) 
all: $(tstexe)

$(txtdir)/cmudic:
	mkdir -p $(txtdir)
	wget http://svn.code.sf.net/p/cmusphinx/code/trunk/cmudict/cmudict-0.7b -O $@


$(lib): $(libsrc)
	mkdir -p $(libdir)
	$(cc) $(cflags) -o $@ $^ -shared -fPIC

$(exe): $(srcdir)/rdabl.c $(lib)
	mkdir -p $(bindir)
	$(cc) $(cflags) -L$(libdir) -o $@ $< -l$(prog)
$(srcdir)/sample_dic_test.c: $(txtdir)/sample_dic
	xxd -i $< > $@

$(srcdir)/sample_test.c: $(txtdir)/sample.txt
	xxd -i $< | sed s/txt_sample_txt/sample_txt/g > $@

$(tstexe): $(tstsrc) $(srcdir)/sample_dic_test.c $(srcdir)/sample_test.c
	$(cc) $(cflags) -L$(libdir) -o $@ $^ -l$(prog)

.PHONY: install
install:
	cp $(lib) $(prefix)/lib
	cp $(exe) $(prefix)/bin

.PHONY: uninstall
uninstall:
	rm $(prefix)/lib/lib$(prog).so
	rm $(prefix)/bin/$(prog)
.PHONY: clean
clean:
	rm -rf $(bindir) $(incdir) $(libdir) $(srcdir) $(txtdir) Makefile *.LICENSE

.PHONY: leaks
leaks:
	valgrind --track-origins=yes ./$(bindir)/$(prog) \
		 -d $(txtdir)/sample_dic $(txtdir)/sample.txt

.PHONY: check
check: $(tstexe)
	./$(tstexe)

.PHONY: demo
demo: $(exe)
	@./$(exe) -d $(txtdir)/sample_dic $(txtdir)/sample.txt
