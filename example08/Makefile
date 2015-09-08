PROGRAM=$(shell basename `pwd`)

all: $(PROGRAM)

OBJS = $(PROGRAM).o main.o

PKG_CONFIG=gl glu
PKG_CONFIG_CFLAGS=`pkg-config --cflags $(PKG_CONFIG) 2>/dev/null`
PKG_CONFIG_LIBS=`pkg-config --libs $(PKG_CONFIG) 2>/dev/null`

CFLAGS= -O2 -g -Wall -D_FORTIFY_SOURCE=2 -std=c++11
DEFS= 
INCS= -I. -I../common

LDFLAGS= -Wl,-z,defs -Wl,-z,relro -Wl,--as-needed -Wl,--no-undefined
LIBS= -L. -lglut -lm -lpthread

$(PROGRAM): $(OBJS)
	g++ $(LDFLAGS) $+ -o $@ $(LIBS) $(PKG_CONFIG_LIBS)

%.o: %.cpp
	g++ -o $@ -c $+ $(DEFS) $(INCS) $(CFLAGS) $(PKG_CONFIG_CFLAGS)

%.o: %.c
	gcc -o $@ -c $+ $(DEFS) $(INCS) $(CFLAGS) $(PKG_CONFIG_CFLAGS)

%.o: ../common/%.cpp
	g++ -o $@ -c $+ $(DEFS) $(INCS) $(CFLAGS) $(PKG_CONFIG_CFLAGS)

%.o: ../common/%.c
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
