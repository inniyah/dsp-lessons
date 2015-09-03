#include "OGLGraph.hpp"

#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cassert>
#include <cmath>
#include <unistd.h>

class PulseGenerator {
public:
	PulseGenerator(int period, double amplitude = 1.0) :
		m_Counter(0),
		m_Period(period),
		m_Amplitude(amplitude)
	{
	}
	double get() {
		return ( ((m_Counter++) % m_Period) < (m_Period/2) ? m_Amplitude : 0.0 );
	}

private:
	int m_Counter;
	int m_Period;
	double m_Amplitude;
};

// Main Application

class App : public IApp<1> {
public:
	App() : pgen(16) {
	}

	virtual void init(void);
	virtual void update(void);

private:
	PulseGenerator pgen;
};

void App::init(void){
}

void App::update(void) {
	Values[0] = pgen.get();
}

// Main application object

static App app;
