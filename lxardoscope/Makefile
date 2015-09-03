#/*
# *
# * This file is part of the LXARDOSCOPE package.
# *
# * LXARDOSCOPE is an Arduino based oscilloscope for Linux, using the Xforms library.
# *
# * Copyright (C) 2011 Oskar Leuthold
# * 
# * LXARDOSCOPE is free software: you can redistribute it and/or modify
# * it under the terms of the GNU General Public License as published by
# * the Free Software Foundation, either version 3 of the License, or
# * (at your option) any later version.
# *
# * LXARDOSCOPE is distributed in the hope that it will be useful, but
# * WITHOUT ANY WARRANTY; without even the implied warranty of
# * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# * Lesser General Public License for more details.
# *
# * You should have received a copy of the GNU Lesser General Public
# * License along with LXARDOSCOPE; see the file COPYING.  If not, write to
# * the Free Software Foundation, 59 Temple Place - Suite 330, Boston,
# * MA 02111-1307, USA.
# *
# */

DESTDIR=
BINDIR=/usr/bin

# Compilation options
CC=gcc
CFLAGS=-g -O2 -Wall -Wno-unused-but-set-variable
CC_OPTIONS=-D_REENTRANT

# Linking libraries
LIBS=-lforms -lX11 -lm -lpthread
LDFLAGS=-Wl,-z,defs  -Wl,-as-needed -Wl,--no-undefined

#*************************************************************************
#**************** Nothing to change after this line **********************
#*************************************************************************


FILE_OUT=lxa
FILES_O=$(FILE_OUT).o $(FILE_OUT)_cb.o $(FILE_OUT)_gui.o $(FILE_OUT)_ard.o $(FILE_OUT)_plt.o $(FILE_OUT)_file.o iscaler.o ipushwheel.o
FILE_EXEC=lxardoscope

#*************************************************************************

all: $(FILES_O)
	$(CC) $(LDFLAGS) $(FILES_O) $(LIBS) -o $(FILE_EXEC)


#*************************************************************************

$(FILE_OUT).o: $(FILE_OUT).c
	$(CC) $(CFLAGS) $(CC_OPTIONS) -c $(FILE_OUT).c -o $(FILE_OUT).o

$(FILE_OUT)_cb.o: $(FILE_OUT)_cb.c
	$(CC) $(CFLAGS) $(CC_OPTIONS) -c $(FILE_OUT)_cb.c -o $(FILE_OUT)_cb.o

$(FILE_OUT)_gui.o: $(FILE_OUT)_gui.c
	$(CC) $(CFLAGS) $(CC_OPTIONS) -c $(FILE_OUT)_gui.c -o $(FILE_OUT)_gui.o

$(FILE_OUT)_ard.o: $(FILE_OUT)_ard.c
	$(CC) $(CFLAGS) $(CC_OPTIONS) -c $(FILE_OUT)_ard.c -o $(FILE_OUT)_ard.o

$(FILE_OUT)_plt.o: $(FILE_OUT)_plt.c
	$(CC) $(CFLAGS) $(CC_OPTIONS) -c $(FILE_OUT)_plt.c -o $(FILE_OUT)_plt.o

$(FILE_OUT)_file.o: $(FILE_OUT)_file.c
	$(CC) $(CFLAGS) $(CC_OPTIONS) -c $(FILE_OUT)_file.c -o $(FILE_OUT)_file.o

iscaler.o: iscaler.c
	$(CC) $(CFLAGS) $(CC_OPTIONS) -c iscaler.c -o iscaler.o

ipushwheel.o: ipushwheel.c
	$(CC) $(CFLAGS) $(CC_OPTIONS) -c ipushwheel.c -o ipushwheel.o

#*************************************************************************

install:
	mkdir -p $(DESTDIR)/$(BINDIR)
	cp -fv lxardoscope $(DESTDIR)/$(BINDIR)

uninstall:
	rm -fv $(DESTDIR)/$(BINDIR)/lxardoscope 

clean:
	rm -fv $(FILES_O)

mrproper: clean
	rm -fv $(FILE_EXEC)


#*************************************************************************
#*************************************************************************
