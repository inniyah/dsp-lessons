/*
 * In this example, we're using a templated filter that can be used to implement any binary
 * operation. In this case, we're implementing something called Composition which will have
 * different versions when compiled against different functions, which it will use to
 * implement binary operations (like add). As our main framework, we're using the
 * interface-based approach we have already explained.
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

	IDataSource & operator>>(double & b) {
		b = get();
		return *this;
	}
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

typedef double (*binary_double_op)(double, double); // Signature for all valid template params

template<binary_double_op OP>
class Composition : public IDataSource {
public:
	Composition(IDataSource & generator1, IDataSource & generator2) :
		m_Generator1(generator1),
		m_Generator2(generator2)
	{
	}
	virtual double get() {
		double input1 = m_Generator1.get();
		double input2 = m_Generator2.get();
		return OP(input1, input2);
	}

private:
	IDataSource & m_Generator1;
	IDataSource & m_Generator2;
};

double add(double a, double b) { return a + b; }

// Main Application

class App : public IApp<1> {
public:
	App() : pgen(30), pflt(pgen), rgen(32), rflt(rgen), cadd(pflt,rflt) {
	}

	virtual void init(void);
	virtual void update(void);

private:
	PulseGenerator   pgen;
	Filter           pflt;
	RampGenerator    rgen;
	Filter           rflt;
	Composition<add> cadd;
};

void App::init(void){
}

void App::update(void) {
	cadd >> Values[0];
}

// Main application object

static App app;
