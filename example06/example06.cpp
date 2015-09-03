#include <cstdint>
#include <cstdlib>
#include <cstdio>
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

int main(int argc, const char * argv[]) {
	unsigned long int Tick = 0;
	PulseGenerator pgen(8);
	Filter         pflt(pgen);
	double         pval;
	RampGenerator  rgen(8);
	Filter         rflt(rgen);
	double         rval;
	while (true) {
		pflt >> pval;
		printf("A\t%lu\t%lf\n", Tick, pval);
		rflt >> rval;
		printf("B\t%lu\t%lf\n", Tick, rval);
		++Tick;
		usleep(100 * 1000lu);
	}
	return EXIT_SUCCESS;
}
