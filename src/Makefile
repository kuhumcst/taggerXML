#  Makefile for Transformation Rule-based Part of Speech Tagger

PNAME=taggerXML
VERSION=3
MINOR_NUMBER=3

#BIN=~/bin/
BIN=

#LIB=/usr/lib64/
#LIB=/usr/lib/
LIB=

PNAMESTATIC=$(BIN)$(PNAME)s
PNAMEDYNAMIC=$(BIN)$(PNAME)
PNAMEDYNAMICLIB=$(BIN)$(PNAME)dl
LINKERNAME=$(LIB)lib$(PNAME).so
SONAME=$(LINKERNAME).$(VERSION)
REALNAME=$(SONAME).$(MINOR_NUMBER)

SRCDIR     = .
HASHDIR    = ../../hashmap/src
LETTERFUNCDIR    = ../../letterfunc/src
SGMLDIR    = ../../parsesgml/src
L_DIR      = .
LIBDIR     = .
BINDIR     = ../Bin_and_Data

GCCINC += -I$(SRCDIR) -I$(HASHDIR) -I$(LETTERFUNCDIR) -I$(SGMLDIR)

CC=g++ -Og -g -Wall -pedantic -DNDEBUG 

# -fPIC or -fpic: enable 'position independent code' generation. Necessary for shared libs
# -fpic may generate smaller and faster code, but will have platform-dependent limitations
PIC=-fPIC
#PIC=

CCLINKSTATIC=g++ -static -fPIC
CCLINKDYNAMIC=g++ -fPIC
CCCREATELIB=g++ -fPIC -shared -Wl,-soname,$(SONAME)
RM=rm -f

include config.mak

CSTTAGGERSRC= \
	main.cpp

TAGGERSRC= \
	$(SRCDIR)/argopt.cpp \
	$(SRCDIR)/darray.cpp \
	$(SRCDIR)/field.cpp \
	$(SRCDIR)/fst.cpp \
	$(HASHDIR)/hashmap.cpp \
	$(LETTERFUNCDIR)/letter.cpp \
	$(LETTERFUNCDIR)/letterfunc.cpp \
	$(SRCDIR)/lex.cpp \
	$(SRCDIR)/memory.cpp \
	$(SRCDIR)/option.cpp \
	$(SGMLDIR)/parsesgml.cpp \
	$(SRCDIR)/readcorpus.cpp \
	$(SRCDIR)/registry.cpp \
	$(SRCDIR)/sst.cpp \
	$(SRCDIR)/substring.cpp \
	$(SRCDIR)/tagger.cpp \
	$(SRCDIR)/text.cpp \
	$(SRCDIR)/useful.cpp \
	$(LETTERFUNCDIR)/utf8func.cpp \
	$(LETTERFUNCDIR)/entities.cpp \
	$(SRCDIR)/word.cpp \
	$(SRCDIR)/XMLtext.cpp

CSTTAGGEROBJS= \
	main.o

TAGGEROBJS=\
	argopt.o \
	darray.o \
	field.o \
	fst.o \
	hashmap.o \
	letter.o \
	letterfunc.o \
	lex.o \
	memory.o \
	option.o \
	parsesgml.o \
	readcorpus.o \
	registry.o \
	sst.o \
	substring.o \
	tagger.o \
	text.o \
	useful.o \
	utf8func.o \
    entities.o \
	word.o \
	XMLtext.o

OBJ = $(LIBDIR)/lex.o \
	$(LIBDIR)/useful.o \
	$(LIBDIR)/darray.o \
	$(LIBDIR)/registry.o \
	$(LETTERFUNCDIR)/letter.o \
	$(LETTERFUNCDIR)/letterfunc.o \
	$(LETTERFUNCDIR)/utf8func.o \
	$(LETTERFUNCDIR)/entities.o \
	$(LIBDIR)/mygets.o \
	$(LIBDIR)/memory.o


L_TARGETS = contextual-rule-learn \
            kbest-contextual-rule-learn \
            fix-kbest-rule-learn

#
#   Dependencies start here.    #
#			       #


$(PNAMEDYNAMIC): $(CSTTAGGEROBJS) $(TAGGEROBJS)
	$(CCLINKDYNAMIC) $(CSTTAGGEROBJS) $(TAGGEROBJS) -o $@

$(PNAMESTATIC): $(CSTTAGGEROBJS) $(TAGGEROBJS)
	$(CCLINKSTATIC) $(CSTTAGGEROBJS) $(TAGGEROBJS) -o $@

$(REALNAME) : $(TAGGEROBJS)
	$(CCCREATELIB) -o $@ $(TAGGEROBJS)
	ln -sf $(REALNAME) $(SONAME)
	ln -sf $(SONAME) $(LINKERNAME)

$(PNAMEDYNAMICLIB): $(REALNAME) $(CSTTAGGEROBJS)
	$(CCLINKDYNAMIC) $(CSTTAGGEROBJS) $(REALNAME) -o $@

$(L_TARGETS) : $(OBJ)
	$(CC) $(PIC) $(OBJ) -o $(BINDIR)/$@ $(L_DIR)/$@.cpp

all: $(PNAMESTATIC) $(PNAMEDYNAMIC) $(REALNAME) $(PNAMEDYNAMICLIB) ${L_TARGETS}

$(TAGGEROBJS): %.o: $(TAGGERSRC)
	$(CC) $(PIC) $(GCCINC) -c $?

$(CSTTAGGEROBJS): %.o: $(CSTTAGGERSRC)
	$(CC) $(PIC) $(GCCINC) -c $?

clean:
	$(RM) *.o
	$(RM) $(REALNAME)
	$(RM) $(SONAME)
	$(RM) $(LINKERNAME)
	$(RM) ../../letterfunc/src/*.o
