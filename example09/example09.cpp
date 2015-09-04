#include "OGLGraph.hpp"
#include "delegate.h"
#include "linked_list.h"

#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cassert>
#include <cmath>
#include <unistd.h>
#include <list>

class AbstractDataSource {
public:
	typedef Delegate<void, double> ListenerDelegate;

	AbstractDataSource() {
	}

	~AbstractDataSource() {
		m_DelegateList.empty();
	}

	inline void insertListener(const ListenerDelegate & delegate) {
		m_DelegateList.push_back( delegate );
	}

	inline void removeListener(const ListenerDelegate & delegate) {
		m_DelegateList.remove( delegate );
	}

	void sendOutput(double value) {
		for(std::list<ListenerDelegate>::iterator iter = m_DelegateList.begin();  iter != m_DelegateList.end(); ++iter) {
			(*iter).operator()(value);
		}
	}

private:
	std::list<ListenerDelegate> m_DelegateList;
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
		m_Generator.insertListener( ListenerDelegate::fromObjectMethod<Filter, &Filter::receive>(this) );
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

class App : public IApp<4> {
public:
	App() : pgen(16), pflt(pgen), rgen(16), rflt(rgen) {
		pgen.insertListener( AbstractDataSource::ListenerDelegate::fromObjectMethod<App, &App::receive_pgen>(this) );
		rgen.insertListener( AbstractDataSource::ListenerDelegate::fromObjectMethod<App, &App::receive_rgen>(this) );
		pflt.insertListener( AbstractDataSource::ListenerDelegate::fromObjectMethod<App, &App::receive_pflt>(this) );
		rflt.insertListener( AbstractDataSource::ListenerDelegate::fromObjectMethod<App, &App::receive_rflt>(this) );
	}

	virtual ~App() {
		pgen.removeListener( AbstractDataSource::ListenerDelegate::fromObjectMethod<App, &App::receive_pgen>(this) );
		rgen.removeListener( AbstractDataSource::ListenerDelegate::fromObjectMethod<App, &App::receive_rgen>(this) );
		pflt.removeListener( AbstractDataSource::ListenerDelegate::fromObjectMethod<App, &App::receive_pflt>(this) );
		rflt.removeListener( AbstractDataSource::ListenerDelegate::fromObjectMethod<App, &App::receive_rflt>(this) );
	}

	void receive_pgen(double input) {
		Values[0] = input;
	}

	void receive_rgen(double input) {
		Values[1] = input;
	}

	void receive_pflt(double input) {
		Values[2] = input;
	}

	void receive_rflt(double input) {
		Values[3] = input;
	}

	virtual void init(void);
	virtual void update(void);

private:
	PulseGenerator pgen;
	Filter         pflt;
	RampGenerator  rgen;
	Filter         rflt;
};

void App::init(void) {
	Graph.LineConfig[0] = Graph.LineConfig[2] * 0.7;
	Graph.LineConfig[0].setStipple(8, 0xEEEE);
	Graph.LineConfig[1] = Graph.LineConfig[3] * 0.7;
	Graph.LineConfig[1].setStipple(8, 0xEEEE);
}

void App::update(void) {
	pgen.go();
	rgen.go();
}

// Main application object

static App app;
