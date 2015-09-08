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

HEADERS = \
	include/DspFilters/RootFinder.h \
	include/DspFilters/State.h \
	include/DspFilters/Filter.h \
	include/DspFilters/Legendre.h \
	include/DspFilters/Layout.h \
	include/DspFilters/PoleFilter.h \
	include/DspFilters/Custom.h \
	include/DspFilters/Bessel.h \
	include/DspFilters/Biquad.h \
	include/DspFilters/Types.h \
	include/DspFilters/RBJ.h \
	include/DspFilters/Utilities.h \
	include/DspFilters/Common.h \
	include/DspFilters/Butterworth.h \
	include/DspFilters/Cascade.h \
	include/DspFilters/ChebyshevII.h \
	include/DspFilters/Elliptic.h \
	include/DspFilters/MathSupplement.h \
	include/DspFilters/Dsp.h \
	include/DspFilters/Design.h \
	include/DspFilters/SmoothedFilter.h \
	include/DspFilters/Params.h \
	include/DspFilters/ChebyshevI.h

SHARED_OBJS = $(addprefix $(SHARED_OBJS_DIR)/,$(addsuffix .o,$(basename $(notdir $(SOURCES)))))
STATIC_OBJS = $(addprefix $(STATIC_OBJS_DIR)/,$(addsuffix .o,$(basename $(notdir $(SOURCES)))))

CFLAGS=-O2 -g -Wall -Wno-unused-function -Wno-unused-but-set-variable
STATIC_CFLAGS= $(CFLAGS)
SHARED_CFLAGS= $(CFLAGS) -fPIC

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
	ar cr $@ $+

%.so : %.o
	g++ $(LDFLAGS) $(LIBS) -shared $^ -o $@

$(SHARED_OBJS_DIR)/%.o: src/%.cpp $(HEADERS)
	g++ -o $@ -c $< $(SHARED_CFLAGS)

$(SHARED_OBJS_DIR)/%.o: src/%.c $(HEADERS)
	gcc -o $@ -c $< $(SHARED_CFLAGS)

$(STATIC_OBJS_DIR)/%.o: src/%.cpp $(HEADERS)
	g++ -o $@ $(DEFS) $(INCS) -c $< $(STATIC_CFLAGS)

$(STATIC_OBJS_DIR)/%.o: src/%.c $(HEADERS)
	gcc -o $@ $(DEFS) $(INCS) -c $< $(STATIC_CFLAGS)

clean:
	rm -fv $(SHARED_OBJS)
	rm -fv $(STATIC_OBJS)
	rm -fv *.so *.so* *.a *~
