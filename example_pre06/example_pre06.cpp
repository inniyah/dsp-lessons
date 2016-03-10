/*
 * This example is intended to clarify the way example06 works. That example uses
 * delegates, which can be confusing for people coming from the C world who are not
 * used to object patterns. In this case, the same paradigm is used, but explicitly
 * with callback functions so that the low level operations can be seen more clarly.
 */

#include "OGLGraph.hpp"

#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cassert>
#include <cmath>
#include <unistd.h>

typedef double (*GetValueCB)(void *);

class PulseGenerator {
public:
	PulseGenerator(int period, double amplitude = 1.0) :
		m_Counter(0),
		m_Period(period),
		m_Amplitude(amplitude)
	{
	}
	static double get(void * user_data) {
		PulseGenerator * ths = reinterpret_cast<PulseGenerator *>(user_data);
		return ( ((ths->m_Counter++) % ths->m_Period) < (ths->m_Period/2) ? ths->m_Amplitude : 0.0 );
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
	static double get(void * user_data) {
		RampGenerator * ths = reinterpret_cast<RampGenerator *>(user_data);
		return ( ((ths->m_Counter++) % ths->m_Period) * ths->m_Amplitude / (ths->m_Period-1) );
	}

private:
	int m_Counter;
	int m_Period;
	double m_Amplitude;
};

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
	Filter(GetValueCB callback, void * user_data, double alpha = 0.5) :
		m_GetValueCallBack(callback),
		m_GetValueUserData(user_data),
		m_Alpha(alpha),
		m_Output(0.0)
	{
	}
	virtual double get() {
		double input = m_GetValueCallBack(m_GetValueUserData);
		m_Output = m_Alpha * input + (1.0 - m_Alpha) * m_Output;
		return m_Output;
	}

private:
	
	GetValueCB   m_GetValueCallBack;
	void       * m_GetValueUserData;
	double       m_Alpha;
	double       m_Output;
};

// Main Application

class App : public IApp<2> {
public:
	App() :
		pgen(16),
		pflt(PulseGenerator::get, &pgen),
		rgen(16),
		rflt(RampGenerator::get, &rgen)
	{
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
