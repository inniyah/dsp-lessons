#include "OGLGraph.hpp"
#include "delegate.h"
#include "flags.h"

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

	AbstractDataSource() : m_Done(false) {
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
		if (!m_Done) {
			for(std::list<ListenerDelegate>::iterator iter = m_DelegateList.begin();  iter != m_DelegateList.end(); ++iter) {
				(*iter).operator()(value);
			}
			m_Done = true;
		}
	}

	void tick() {
		if (m_Done) {
			m_Done = false;
		}
	}

	void reset() {
		m_Done = false;
	}

private:
	std::list<ListenerDelegate> m_DelegateList;
	bool m_Done;
};

class PulseGenerator : public AbstractDataSource {
public:
	PulseGenerator() : m_Counter(0), m_Period(0), m_Amplitude(0.0) {
	}

	void setup(unsigned int period, double amplitude = 1.0, unsigned int initial_counter = 0) {
		m_Period = period;
		m_Amplitude = amplitude;
		m_Counter = (initial_counter % m_Period);
	}

	void go() {
		if (0 != m_Period) {
			sendOutput( ((m_Counter++) % m_Period) < (m_Period/2) ? m_Amplitude : 0.0 );
		}
	}

private:
	unsigned int m_Counter;
	unsigned int m_Period;
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
	Filter() : m_Source(NULL), m_Alpha(1.0), m_Ampli(1.0), m_Output(0.0) {
	}

	void setup(AbstractDataSource & source, double alpha = 0.5, double amplification = 1.0) {
		AbstractDataSource::reset();

		if (NULL != m_Source) {
			m_Source->removeListener( ListenerDelegate::fromObjectMethod<Filter, &Filter::receive>(this) );
		}

		m_Source = &source;
		m_Alpha = alpha;
		m_Ampli = amplification;
		m_Output = 0.0;

		if (NULL != m_Source) {
			m_Source->insertListener( ListenerDelegate::fromObjectMethod<Filter, &Filter::receive>(this) );
		}
	}

	virtual ~Filter() {
		if (NULL != m_Source) {
			m_Source->removeListener( ListenerDelegate::fromObjectMethod<Filter, &Filter::receive>(this) );
		}
	}

	void receive(double input) {
		m_Output = m_Alpha * input + (1.0 - m_Alpha) * m_Output;
		sendOutput( m_Ampli * m_Output );
	}

private:
	AbstractDataSource * m_Source;
	double m_Alpha;
	double m_Ampli;
	double m_Output;
};


typedef double (*binary_double_op)(double, double); // Signature for all valid template params

template<binary_double_op OP>
class Composition : public AbstractDataSource {
public:
	Composition() : m_Source1(NULL), m_Source2(NULL), m_InputsReceived(m_InputsReceived.None), m_Input1(std::nan("")), m_Input2(std::nan("")) {
	}

	void setup(AbstractDataSource & source1, AbstractDataSource & source2) {
		AbstractDataSource::reset();

		if (NULL != m_Source1) {
			m_Source1->removeListener( ListenerDelegate::fromObjectMethod<Composition, &Composition::receive_gen1>(this) );
		}
		if (NULL != m_Source2) {
			m_Source2->removeListener( ListenerDelegate::fromObjectMethod<Composition, &Composition::receive_gen2>(this) );
		}

		m_Source1 = &source1;
		m_Source2 = &source2;

		if (NULL != m_Source1) {
			m_Source1->insertListener( ListenerDelegate::fromObjectMethod<Composition, &Composition::receive_gen1>(this) );
		}
		if (NULL != m_Source2) {
			m_Source2->insertListener( ListenerDelegate::fromObjectMethod<Composition, &Composition::receive_gen2>(this) );
		}

		m_InputsReceived.setAll();
		m_Input1 = 0.0;
		m_Input2 = 0.0;
	}

	void receive_gen1(double input) {
		m_Input1 = input;
		m_InputsReceived.set(0);
		sendCurrentOutputIfReady();
	}

	void receive_gen2(double input) {
		m_Input2 = input;
		m_InputsReceived.set(1);
		sendCurrentOutputIfReady();
	}

private:
	void sendCurrentOutputIfReady() {
		if (m_InputsReceived.isFull()) {
			m_InputsReceived.resetAll();
			double output = OP(m_Input1, m_Input2);
			sendOutput( output );
		}
	}

	AbstractDataSource * m_Source1;
	AbstractDataSource * m_Source2;
	Flags<unsigned char, 0x03> m_InputsReceived;
	double m_Input1;
	double m_Input2;
};

double sub(double a, double b) { return a - b; }

// Main Application

class App : public IApp<3> {
public:
	App() {
		pgen.setup(100);
		csub.setup(pgen, proc);
		proc.setup(csub, 1.0, 0.6);

		pgen.insertListener( AbstractDataSource::ListenerDelegate::fromObjectMethod<App, &App::receive_pgen>(this) );
		csub.insertListener( AbstractDataSource::ListenerDelegate::fromObjectMethod<App, &App::receive_csub>(this) );
		proc.insertListener( AbstractDataSource::ListenerDelegate::fromObjectMethod<App, &App::receive_proc>(this) );
	}

	virtual ~App() {
		pgen.removeListener( AbstractDataSource::ListenerDelegate::fromObjectMethod<App, &App::receive_pgen>(this) );
		csub.removeListener( AbstractDataSource::ListenerDelegate::fromObjectMethod<App, &App::receive_csub>(this) );
		proc.removeListener( AbstractDataSource::ListenerDelegate::fromObjectMethod<App, &App::receive_proc>(this) );
	}

	void receive_pgen(double input) {
		Values[0] = input;
	}

	void receive_csub(double input) {
		Values[1] = input;
	}

	void receive_proc(double input) {
		Values[2] = input;
	}

	void tick() {
		pgen.tick();
		csub.tick();
		proc.tick();
	}

	virtual void init(void);
	virtual void update(void);

private:
	PulseGenerator pgen;
	Composition<sub> csub;
	Filter proc;
};

void App::init(void) {
}

void App::update(void) {
	tick();
	pgen.go();
}

// Main application object

static App app;
