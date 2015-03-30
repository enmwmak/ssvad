#####################################################################
# FILE		:Makefile 
# AUTHOR	:M.W. Mak (mwmak@polyu.edu.hk)
#####################################################################

# Users needed to change the Project Directory here
PRJDIR=$(PWD)/..
INCLUDEDIR=$(PRJDIR)/src
TARGETDIR=$(PRJDIR)/bin

# SPHERE Library (http://www.nist.gov/itl/iad/mig/tools.cfm)
NISTDIR=/usr/local/nist2.7
NISTINCLUDEDIR=$(NISTDIR)/include
NISTLIBDIR=$(NISTDIR)/lib

# Compiling options
CC = gcc
CFLAG = -c -Wall -Werror -DVECTOR_TYPE=double -DVFORMAT=\"%lf\"
MATHLIB = -lm
NISTLIB = -lsp -lutil

# Object and target files
OBJS = sph_io.o mmalloc.o veclib.o cmdline.o qsortfunc.o rm_crosstalk.o \
       segment.o fft.o silence.o denoise.o findnoise.o window.o winwav.o

TARGET1 = sph2phn
TARGET2 = sph2phn_2ch
TARGET3 = phninfo

$(TARGETDIR)/$(TARGET1): $(OBJS) $(TARGET1).o
	$(CC) -o $@ $(OBJS) $(TARGET1).o -L$(NISTLIBDIR) $(NISTLIB) $(MATHLIB)

$(TARGETDIR)/$(TARGET2): $(OBJS) $(TARGET2).o
	$(CC) -o $@ $(OBJS) $(TARGET2).o -L$(NISTLIBDIR) $(NISTLIB) $(MATHLIB)

$(TARGETDIR)/$(TARGET3): $(OBJS) $(TARGET3).o
	$(CC) -o $@ $(OBJS) $(TARGET3).o -L$(NISTLIBDIR) $(NISTLIB) $(MATHLIB)


all::	$(TARGETDIR)/$(TARGET1) \
	$(TARGETDIR)/$(TARGET2) \
	$(TARGETDIR)/$(TARGET3)

clean: 
	rm -f *.o *~ $(TARGETDIR)/*

#Define inference rules
.c.o:
	$(CC) $(CFLAG) -I$(INCLUDEDIR) -I$(NISTINCLUDEDIR) $*.c

#Dependency
sph_io.o: sph_io.c $(INCLUDEDIR)/winpara.h $(INCLUDEDIR)/sph_io.h
mmalloc.o: mmalloc.c $(INCLUDEDIR)/mmalloc.h
veclib.o: veclib.c $(INCLUDEDIR)/veclib.h
window.o: window.c $(INCLUDEDIR)/window.h
cmdline.o: cmdline.c $(INCLUDEDIR)/cmdline.h
segment.o: segment.c $(INCLUDEDIR)/segment.h
winwav.o: winwav.c $(INCLUDEDIR)/winwav.h
sph2phn.o: sph2phn.c
silence.o: silence.c $(INCLUDEDIR)/silence.h
qsortfunc.o: qsortfunc.c
fft.o: fft.c $(INCLUDEDIR)/fft.h
phninfo.o: phninfo.c
denoise.o: denoise.c
findnoise.o: findnoise.c
wav2phn.o: wav2phn.c
sph2phn_2ch: sph2phn_2ch.c
mfc2mfc: mfc2mfc.c

