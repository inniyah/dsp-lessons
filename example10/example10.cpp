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

private:
	std::list<ListenerDelegate> m_DelegateList;
	bool m_Done;
};

class PulseGenerator : public AbstractDataSource {
public:
	PulseGenerator(int period, double amplitude = 1.0) :
		m_Counter(0),
		m_Period(period),
		m_Amplitude(amplitude)
	{
	}

	void setup() {
	}

	void go() {
		sendOutput( ((m_Counter++) % m_Period) < (m_Period/2) ? m_Amplitude : 0.0 );
	}

private:
	int m_Counter;
	int m_Period;
	double m_Amplitude;
};

template<typename N, N MASK = ~0>
class Flags {
public:
	typedef N Type;
	static const unsigned int Bits = sizeof(N)*8;
	static const N Mask            = static_cast<N>(MASK);
	static const N None            = static_cast<N>(0) & Mask;
	static const N All             = static_cast<N>(~0) & Mask;

	Flags(N ini = None) : m_Data(ini) {
	}

	inline bool get(unsigned int bit) {
		return (0 != (m_Data & static_cast<N>(1 << bit)));
	}

	inline void set(unsigned int bit) {
		m_Data |= static_cast<N>(1 << bit);
	}

	inline void reset(unsigned int bit) {
		m_Data &= static_cast<N>(~(1 << bit));
	}

	inline void toggle(unsigned int bit) {
		m_Data ^= static_cast<N>(1 << bit);
	}

	inline bool isEmpty() {
		return (None == m_Data);
	}

	inline bool isFull() {
		return (All == m_Data);
	}

private:
	N m_Data;
};


class MyProcess : public AbstractDataSource {
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
	MyProcess(AbstractDataSource & source) : m_Source(source) {
	}

	void setup() {
		m_Source.insertListener( ListenerDelegate::fromObjectMethod<MyProcess, &MyProcess::receive>(this) );
	}

	virtual ~MyProcess() {
		m_Source.removeListener( ListenerDelegate::fromObjectMethod<MyProcess, &MyProcess::receive>(this) );
	}

	void receive(double input) {
		sendOutput( 2.0 * input );
	}

private:
	AbstractDataSource & m_Source;
};


typedef double (*binary_double_op)(double, double); // Signature for all valid template params

template<binary_double_op OP>
class Composition : public AbstractDataSource {
public:
	Composition(AbstractDataSource & source1, AbstractDataSource & source2) :
		m_Source1(source1),
		m_Source2(source2),
		m_InputsReceived(m_InputsReceived.All),
		m_Input1(std::nan("")),
		m_Input2(std::nan(""))
	{
	}

	void setup() {
		m_Source1.insertListener( AbstractDataSource::ListenerDelegate::fromObjectMethod<Composition, &Composition::receive_gen1>(this) );
		m_Source2.insertListener( AbstractDataSource::ListenerDelegate::fromObjectMethod<Composition, &Composition::receive_gen2>(this) );
	}

	void receive_gen1(double input) {
		m_Input1 = input;
		m_InputsReceived.set(0);
		printf("A\n");
		sendCurrentOutputIfReady();
	}

	void receive_gen2(double input) {
		m_Input2 = input;
		m_InputsReceived.set(1);
		printf("B\n");
		sendCurrentOutputIfReady();
	}

private:
	void sendCurrentOutputIfReady() {
		if (m_InputsReceived.isFull()) {
			double output = OP(m_Input1, m_Input2);
			sendOutput( output );
			printf("****\n");
		}
	}

	AbstractDataSource & m_Source1;
	AbstractDataSource & m_Source2;
	Flags<unsigned char, 0x03> m_InputsReceived;
	double m_Input1;
	double m_Input2;
};

double sub(double a, double b) { return a - b; }

// Main Application

class App : public IApp<3> {
public:
	App() : pgen(100), csub(pgen, proc), proc(csub) {
		pgen.setup();
		csub.setup();
		proc.setup();
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
	}

	virtual void init(void);
	virtual void update(void);

private:
	PulseGenerator pgen;
	Composition<sub> csub;
	MyProcess proc;
};

void App::init(void) {
}

void App::update(void) {
	tick();
	pgen.go();
}

// Main application object

static App app;
