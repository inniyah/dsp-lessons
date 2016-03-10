/*
 * This is a very simple example of a signal generator, in this case a pulse
 * generator. The main application consists in a non-stop loop that gets a value
 * from the generator, prints it on the screen, and waits for a bit until getting\
 * a new value.
 */

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

int main(int argc, const char * argv[]) {
	PulseGenerator pgen(16);
	while (true) {
		printf("%.2lf\n", pgen.get());
		usleep(100000u);
	}
	return EXIT_SUCCESS;
}
