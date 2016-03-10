/*
 * The first proposal to achieve a filter that's independent of the type of generator
 * that we're using is by using pure virtual classes with virtual functions to provide
 * something equivalent to interfaces. In this case we're defining an interface IDataSource
 * with a function get that all the data sources will inherit. The filter, then, will only
 * have to depend on the generator implementing that interface. Please acknowledge that, in
 * the case of C++, this will cause that all generators will have a reference to an vtable and
 * also that whoever calls get will have to resolve two pointer indirections to do so (obtain the
 * vtable from the object, and obtain the function from the vtable).
 * In this example we're implementing two different generators: the pulse generator that we already
 * had an a ramp generator.
 */

#include "OGLGraph.hpp"

#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cassert>
#include <cmath>
#include <unistd.h>

struct IDataSource {
	virtual double get() = 0;
};

class PulseGenerator : public IDataSource {
public:
	PulseGenerator(int period, double amplitude = 1.0) :
		m_Counter(0),
		m_Period(period),
		m_Amplitude(amplitude)
	{
	}
	virtual double get() {
		return ( ((m_Counter++) % m_Period) < (m_Period/2) ? m_Amplitude : 0.0 );
	}

private:
	int m_Counter;
	int m_Period;
	double m_Amplitude;
};

class RampGenerator : public IDataSource {
public:
	RampGenerator(int period, double amplitude = 1.0) :
		m_Counter(0),
		m_Period(period),
		m_Amplitude(amplitude)
	{
	}
	virtual double get() {
		return ( ((m_Counter++) % m_Period) * m_Amplitude / (m_Period-1) );
	}

private:
	int m_Counter;
	int m_Period;
	double m_Amplitude;
};

class Filter : public IDataSource {
	/*
	 * first order IIR filters to approximate a K sample moving average.
	 * This function implements the equation:
	 *
	 *                  y[n] = alpha * x[n] + (1 - alpha) * y[n-1]
	 *
	 * This filter approximates a moving average of the last K samples
	 * by setting the value of alpha to 1/K.
	 *
	 * See: http://electronics.stackexchange.com/questions/30370/fast-and-memory-efficient-moving-average-calculation
	 * See: http://dsp.stackexchange.com/questions/378/what-is-the-best-first-order-iir-approximation-to-a-moving-average-filter
	 */
public:
	Filter(IDataSource & generator, double alpha = 0.5) :
		m_Generator(generator),
		m_Alpha(alpha),
		m_Output(0.0)
	{
	}
	virtual double get() {
		double input = m_Generator.get();
		m_Output = m_Alpha * input + (1.0 - m_Alpha) * m_Output;
		return m_Output;
	}

private:
	IDataSource & m_Generator;
	double m_Alpha;
	double m_Output;
};

// Main Application

class App : public IApp<2> {
public:
	App() : pgen(16), pflt(pgen), rgen(16), rflt(rgen) {
	}

	virtual void init(void);
	virtual void update(void);

private:
	PulseGenerator pgen;
	Filter         pflt;
	RampGenerator  rgen;
	Filter         rflt;
};

void App::init(void){
}

void App::update(void) {
	Values[0] = pflt.get();
	Values[1] = rflt.get();
}

// Main application object

static App app;
