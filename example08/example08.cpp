/*
 * In the previous examples, we have always had a signal dump that extracted the data
 * from something (a data generator or a filter), which in turn recursively got the data
 * from whatever sources it depends on. We're changing that paradigm in this script: it
 * will be the sources who push the data (with a function called go), which will propagate
 * throughout the next layers until they reach a dump. Notice that, in this example, a data
 * source will just be able to send data to one data receiver. It won't support two filters
 * getting the data from the same data source, for example.
 */

#include "OGLGraph.hpp"
#include "delegate.h"

#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cassert>
#include <cmath>
#include <unistd.h>

class AbstractDataSource {
public:
	typedef Delegate<void, double> ListenerDelegate;

	AbstractDataSource() :
		m_ListenerDelegate(ListenerDelegate::fromObjectMethod<AbstractDataSource, &AbstractDataSource::emptyListenerDelegate>(this))
	{
	}

	void emptyListenerDelegate(double) {
	}

	void setListener(ListenerDelegate delegate) {
		m_ListenerDelegate = delegate;
	}

	void removeListener(ListenerDelegate delegate) {
		assert(m_ListenerDelegate == delegate);
		m_ListenerDelegate = ListenerDelegate::fromObjectMethod<AbstractDataSource, &AbstractDataSource::emptyListenerDelegate>(this);
	}

	void sendOutput(double value) {
		m_ListenerDelegate(value);
	}

private:
	ListenerDelegate m_ListenerDelegate;
};

class PulseGenerator : public AbstractDataSource {
public:
	PulseGenerator(int period, double amplitude = 1.0) :
		m_Counter(0),
		m_Period(period),
		m_Amplitude(amplitude)
	{
	}

	void go() {
		sendOutput( ((m_Counter++) % m_Period) < (m_Period/2) ? m_Amplitude : 0.0 );
	}

private:
	int m_Counter;
	int m_Period;
	double m_Amplitude;
};

class RampGenerator : public AbstractDataSource {
public:
	RampGenerator(int period, double amplitude = 1.0) :
		m_Counter(0),
		m_Period(period),
		m_Amplitude(amplitude)
	{
	}

	void go() {
		sendOutput( ((m_Counter++) % m_Period) * m_Amplitude / (m_Period-1) );
	}

private:
	int m_Counter;
	int m_Period;
	double m_Amplitude;
};

class Filter : public AbstractDataSource {
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
	Filter(AbstractDataSource & generator, double alpha = 0.5) :
		m_Generator(generator),
		m_Alpha(alpha),
		m_Output(0.0)
	{
		m_Generator.setListener( ListenerDelegate::fromObjectMethod<Filter, &Filter::receive>(this) );
	}

	virtual ~Filter() {
		m_Generator.removeListener( ListenerDelegate::fromObjectMethod<Filter, &Filter::receive>(this) );
	}

	void receive(double input) {
		m_Output = m_Alpha * input + (1.0 - m_Alpha) * m_Output;
		sendOutput( m_Output );
	}

private:
	AbstractDataSource & m_Generator;
	double m_Alpha;
	double m_Output;
};

// Main Application

class App : public IApp<2> {
public:
	App() : pgen(16), pflt(pgen), rgen(16), rflt(rgen) {
		pflt.setListener( AbstractDataSource::ListenerDelegate::fromObjectMethod<App, &App::receive_pflt>(this) );
		rflt.setListener( AbstractDataSource::ListenerDelegate::fromObjectMethod<App, &App::receive_rflt>(this) );
	}

	virtual ~App() {
		pflt.removeListener( AbstractDataSource::ListenerDelegate::fromObjectMethod<App, &App::receive_pflt>(this) );
		rflt.removeListener( AbstractDataSource::ListenerDelegate::fromObjectMethod<App, &App::receive_rflt>(this) );
	}

	void receive_pflt(double input) {
		Values[0] = input;
	}

	void receive_rflt(double input) {
		Values[1] = input;
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
	pgen.go();
	rgen.go();
}

// Main application object

static App app;
