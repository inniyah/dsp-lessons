#include <cstdint>
#include <cstdlib>
#include <cstdio>
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

int main(int argc, const char * argv[]) {
	unsigned long int Tick = 0;
	PulseGenerator pgen(8);
	while (true) {
		printf("A\t%lu\t%lf\n", Tick++, pgen.get());
		usleep(100 * 1000lu);
	}
	return EXIT_SUCCESS;
}