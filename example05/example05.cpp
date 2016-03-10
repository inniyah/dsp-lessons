/*
 * In this example we're providing a totally different way to deal with the problem
 * of decoupling the filter from the type of generator. For this, we will be using
 * templates in the definition of the filter. This means that, when the program is
 * compiled, a different filter will exist for every kind og generator that is being
 * used, and that means that there will be more code. It also means that when the filters
 * are compiled, the generators' code has to be accessible. On the good part, the generated
 * code is as quick and fast as in our first examples, without any extra operations to call
 * the get function.
 */

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

class RampGenerator {
public:
	RampGenerator(int period, double amplitude = 1.0) :
		m_Counter(0),
		m_Period(period),
		m_Amplitude(amplitude)
	{
	}
	double get() {
		return ( ((m_Counter++) % m_Period) * m_Amplitude / (m_Period-1) );
	}

private:
	int m_Counter;
	int m_Period;
	double m_Amplitude;
};

template<typename T>
class Filter {
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
	Filter(T & generator, double alpha = 0.5) :
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
	T & m_Generator;
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
	PulseGenerator         pgen;
	Filter<PulseGenerator> pflt;
	RampGenerator          rgen;
	Filter<RampGenerator>  rflt;
};

void App::init(void){
}

void App::update(void) {
	Values[0] = pflt.get();
	Values[1] = rflt.get();
}

// Main application object

static App app;
