LIBRARY=libDspFilters

all: static

SHARED_OBJS_DIR=shared_objs
STATIC_OBJS_DIR=static_objs

$(SHARED_OBJS_DIR):
	mkdir -p $@

$(STATIC_OBJS_DIR):
	mkdir -p $@

static: $(STATIC_OBJS_DIR) $(LIBRARY).a
shared: $(SHARED_OBJS_DIR) $(LIBRARY).so

MAJOR=0
MINOR=0

SOURCES = \
	Bessel.cpp \
	Butterworth.cpp \
	ChebyshevI.cpp \
	Custom.cpp \
	Documentation.cpp \
	Filter.cpp \
	Param.cpp \
	RBJ.cpp \
	State.cpp \
	Biquad.cpp \
	Cascade.cpp \
	ChebyshevII.cpp \
	Design.cpp \
	Elliptic.cpp \
	Legendre.cpp \
	PoleFilter.cpp \
	RootFinder.cpp

SHARED_OBJS = $(addprefix $(SHARED_OBJS_DIR)/,$(addsuffix .o,$(basename $(notdir $(SOURCES)))))
STATIC_OBJS = $(addprefix $(STATIC_OBJS_DIR)/,$(addsuffix .o,$(basename $(notdir $(SOURCES)))))

CFLAGS=-O2 -g -Wall 
STATIC_CFLAGS= -O2 -g -Wall $(CFLAGS)
SHARED_CFLAGS= $(STATIC_CFLAGS) -fPIC

LDFLAGS= -Wl,-z,defs -Wl,--as-needed -Wl,--no-undefined
LIBS=
INCS=-Iinclude
DEFS=

$(LIBRARY).so.$(MAJOR).$(MINOR): $(SHARED_OBJS)
	g++ $(LDFLAGS) $(EXTRA_LDFLAGS) -shared \
		-Wl,-soname,$(LIBRARY).so.$(MAJOR) \
		-o $(LIBRARY).so.$(MAJOR).$(MINOR) \
		$+ -o $@ $(LIBS)

$(LIBRARY).so: $(LIBRARY).so.$(MAJOR).$(MINOR)
	rm -f $@.$(MAJOR)
	ln -s $@.$(MAJOR).$(MINOR) $@.$(MAJOR)
	rm -f $@
	ln -s $@.$(MAJOR) $@

$(LIBRARY).a: $(STATIC_OBJS)
	ar cru $@ $+

%.so : %.o
	g++ $(LDFLAGS) $(LIBS) -shared $^ -o $@

$(SHARED_OBJS_DIR)/%.o: src/%.cpp
	g++ -o $@ -c $+ $(SHARED_CFLAGS)

$(SHARED_OBJS_DIR)/%.o: src/%.c
	gcc -o $@ -c $+ $(SHARED_CFLAGS)

$(STATIC_OBJS_DIR)/%.o: src/%.cpp
	g++ -o $@ $(DEFS) $(INCS) -c $+ $(STATIC_CFLAGS)

$(STATIC_OBJS_DIR)/%.o: src/%.c
	gcc -o $@ $(DEFS) $(INCS) -c $+ $(STATIC_CFLAGS)

%.tab.c %.tab.h: model/%.y
	bison -d $<

%.yy.c: model/%.l %.tab.h
	flex -o $@ $<

clean:
	rm -fv $(SHARED_OBJS)
	rm -fv $(STATIC_OBJS)
	rm -f *.tab.c *.tab.h *.yy.c *.so *.so* *.a *~
