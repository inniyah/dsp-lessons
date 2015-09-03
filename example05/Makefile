PROGRAM=$(shell basename `pwd`)

all: $(PROGRAM)

OBJS = $(PROGRAM).o

PKG_CONFIG=
PKG_CONFIG_CFLAGS=`pkg-config --cflags $(PKG_CONFIG) 2>/dev/null` `fltk-config --cflags`
PKG_CONFIG_LIBS=`pkg-config --libs $(PKG_CONFIG) 2>/dev/null` `fltk-config --ldflags --libs`

CFLAGS= -O2 -g -Wall -std=c++11
DEFS= 
INCS= -I.

LDFLAGS= -Wl,-z,defs -Wl,--as-needed -Wl,--no-undefined
LIBS= -L.

$(PROGRAM): $(OBJS)
	g++ $(LDFLAGS) $+ -o $@ $(LIBS) $(PKG_CONFIG_LIBS)

%.o: %.cpp
	g++ -o $@ -c $+ $(DEFS) $(INCS) $(CFLAGS) $(PKG_CONFIG_CFLAGS)

%.o: %.c
	gcc -o $@ -c $+ $(DEFS) $(INCS) $(CFLAGS) $(PKG_CONFIG_CFLAGS)

%.h %.cpp: %.fl
	fluid -c $<

%.tab.c %.tab.h: %.y
	bison -d $<

%.yy.c: %.l %.tab.h
	flex -o $@ $<

clean:
	rm -fv $(OBJS)
	rm -fv $(PROGRAM)
	rm -fv $(addsuffix .h,$(basename $(shell find . -maxdepth 1 -name "*.fl")))
	rm -fv $(addsuffix .cpp,$(basename $(shell find . -maxdepth 1 -name "*.fl")))
	rm -fv *.o *.a *~
