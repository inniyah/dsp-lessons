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

class RampGenerator : public AbstractDataSource {
public:
	RampGenerator() : m_Counter(0), m_Period(0), m_Amplitude(0.0) {
	}

	void setup(unsigned int period, double amplitude = 1.0, unsigned int initial_counter = 0) {
		m_Period = period;
		m_Amplitude = amplitude;
		m_Counter = (initial_counter % m_Period);
	}

	void go() {
		if (0 != m_Period) {
			sendOutput( ((m_Counter++) % m_Period) * m_Amplitude / (m_Period-1) );
		}
	}

private:
	int m_Counter;
	int m_Period;
	double m_Amplitude;
};

class FilterStateMachine : public AbstractDataSource {
public:
	FilterStateMachine() : m_pCurrentState(&s_SleepState), m_Input(NULL), m_Control(NULL) {
	}

	void setup(AbstractDataSource & input, AbstractDataSource & control) {
		AbstractDataSource::reset();

		if (NULL != m_Input) {
			m_Input->removeListener( ListenerDelegate::fromObjectMethod<FilterStateMachine, &FilterStateMachine::receive_input>(this) );
		}
		if (NULL != m_Control) {
			m_Control->removeListener( ListenerDelegate::fromObjectMethod<FilterStateMachine, &FilterStateMachine::receive_control>(this) );
		}

		m_Input   = &input;
		m_Control = &control;

		if (NULL != m_Input) {
			m_Input->insertListener( ListenerDelegate::fromObjectMethod<FilterStateMachine, &FilterStateMachine::receive_input>(this) );
		}
		if (NULL != m_Control) {
			m_Control->insertListener( ListenerDelegate::fromObjectMethod<FilterStateMachine, &FilterStateMachine::receive_control>(this) );
		}
	}

	virtual ~FilterStateMachine() {
		if (NULL != m_Input) {
			m_Input->removeListener( ListenerDelegate::fromObjectMethod<FilterStateMachine, &FilterStateMachine::receive_input>(this) );
		}
		if (NULL != m_Control) {
			m_Control->removeListener( ListenerDelegate::fromObjectMethod<FilterStateMachine, &FilterStateMachine::receive_control>(this) );
		}
	}
	void receive_input(double input) {
		m_pCurrentState->receive_input(*this, input);
	}

	void receive_control(double control) {
		m_pCurrentState->receive_control(*this, control);
	}

private:
	struct AbstractState {
		virtual void enter(FilterStateMachine & sm) { }
		virtual void exit(FilterStateMachine & sm) { }
		virtual void receive_input(FilterStateMachine & sm, double input) = 0;
		virtual void receive_control(FilterStateMachine & sm, double control) = 0;
	};

	struct SleepState : public AbstractState {
		virtual void receive_input(FilterStateMachine & sm, double input) {
			sm.sendOutput(0.0);
		}
		virtual void receive_control(FilterStateMachine & sm, double control) {
			if (control > sm.m_PrevControl) {
				sm.changeState(s_ActiveState);
			}
			sm.m_PrevControl = control;
		}
	};

	struct ActiveState : public AbstractState {
		virtual void receive_input(FilterStateMachine & sm, double input) {
			sm.sendOutput(1.0 + input);
		}
		virtual void receive_control(FilterStateMachine & sm, double control) {
			if (control > sm.m_PrevControl) {
				sm.changeState(s_SleepState);
			}
			sm.m_PrevControl = control;
		}
	};

	void changeState(AbstractState & new_state) {
		m_pCurrentState->exit(*this);
		m_pCurrentState = &new_state;
		m_pCurrentState->enter(*this);
	}

	static SleepState  s_SleepState;
	static ActiveState s_ActiveState;

	AbstractState * m_pCurrentState;

	AbstractDataSource * m_Input;
	AbstractDataSource * m_Control;

	double m_PrevControl;
};

FilterStateMachine::SleepState  FilterStateMachine::s_SleepState;
FilterStateMachine::ActiveState FilterStateMachine::s_ActiveState;

// Main Application

class App : public IApp<3> {
public:
	App() {
		pgen.setup(100);
		rgen.setup(73, 10.0);
		filt.setup(rgen, pgen);

		pgen.insertListener( AbstractDataSource::ListenerDelegate::fromObjectMethod<App, &App::receive_pgen>(this) );
		rgen.insertListener( AbstractDataSource::ListenerDelegate::fromObjectMethod<App, &App::receive_rgen>(this) );
		filt.insertListener( AbstractDataSource::ListenerDelegate::fromObjectMethod<App, &App::receive_filt>(this) );
	}

	virtual ~App() {
		pgen.removeListener( AbstractDataSource::ListenerDelegate::fromObjectMethod<App, &App::receive_pgen>(this) );
		rgen.removeListener( AbstractDataSource::ListenerDelegate::fromObjectMethod<App, &App::receive_rgen>(this) );
		filt.removeListener( AbstractDataSource::ListenerDelegate::fromObjectMethod<App, &App::receive_filt>(this) );
	}

	void receive_pgen(double input) {
		Values[0] = input;
	}

	void receive_rgen(double input) {
		Values[1] = input;
	}

	void receive_filt(double input) {
		Values[2] = input;
	}

	void tick() {
		pgen.tick();
		rgen.tick();
		filt.tick();
	}

	virtual void init(void);
	virtual void update(void);

private:
	PulseGenerator     pgen;
	RampGenerator      rgen;
	FilterStateMachine filt;
};

void App::init(void) {
}

void App::update(void) {
	tick();
	pgen.go();
	rgen.go();
}

// Main application object

static App app;
