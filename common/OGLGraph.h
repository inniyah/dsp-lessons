/*
    Header file for OGLGraph class
    Copyright (C) 2006 Gabriyel Wong (gabriyel@gmail.com)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <ctime>
#include <cassert>
#include <cstring>
#include <cmath>
#include <list>

template<unsigned int CHANNELS>
class OGLGraph {
public:
	void setup(int cache_size, double min_value = -10.0, double max_value = 10.0);
	void update(const double data[CHANNELS]);
	void draw();

	struct LineConfigS {
		LineConfigS() : Red(0), Green(0), Blue(0), StippleFactor(0), StipplePattern(0xFFFF) {
			unsigned char counter = ++Counter;
			counter = (counter ^~0) & 0x1FF;
			if (
				(!counter) ||
				((counter & 0x7)   == 0x7) ||
				((counter & 0x3F)  == 0x38) ||
				((counter & 0x1FF) == 0x1E0) ) {
				++Counter;
				counter = (counter ^~0) & 0x1FF;
			}
			for (unsigned int i = 0; i < 3; ++i) {
				Blue  = Blue  | ((counter & 1) << (7-i));
				counter >>= 1;
				Red   = Red   | ((counter & 1) << (7-i));
				counter >>= 1;
				Green = Green | ((counter & 1) << (7-i));
				counter >>= 1;
			}
		}
		void set(unsigned char r, unsigned char g, unsigned char b, int stf = 0, unsigned short stp = 0xFFFF) {
			Red = r;
			Green = g;
			Blue = b;
			StippleFactor = stf;
			StipplePattern = stp;
		}
		void setStipple(int stf = 0, unsigned short stp = 0xFFFF) {
			StippleFactor = stf;
			StipplePattern = stp;
		}
		LineConfigS & operator=(const LineConfigS & lc) {
			Red = lc.Red;
			Green = lc.Green;
			Blue = lc.Blue;
			StippleFactor = lc.StippleFactor;
			StipplePattern = lc.StipplePattern;
			return *this;
		}
		LineConfigS operator*(float factor) const {
			LineConfigS lc(*this);
			lc.Red = static_cast<unsigned char>(::fmax(0.0, ::fmin(255.0, factor * static_cast<float>(Red))));
			lc.Green = static_cast<unsigned char>(::fmax(0.0, ::fmin(255.0, factor * static_cast<float>(Green))));
			lc.Blue = static_cast<unsigned char>(::fmax(0.0, ::fmin(255.0, factor * static_cast<float>(Blue))));
			return lc;
		}
		unsigned char Red, Green, Blue;
		int StippleFactor;
		unsigned short StipplePattern;
	private:
		static unsigned char Counter;
	};

	LineConfigS LineConfig[CHANNELS];

private:
	int _cacheSize;
	std::list<float> _data[CHANNELS];
	double _maxValue;
	double _minValue;
	double _maxDetectedValue;
	double _minDetectedValue;
};

struct IBaseApp {
	IBaseApp() {
		assert(NULL == MainApp);
		MainApp = this;
	}

	virtual void setup(void) = 0;
	virtual void init(void) = 0;
	virtual void update(void) = 0;

	virtual void refresh(void) = 0;
	virtual void draw(void) = 0;

	static IBaseApp * MainApp;
};

template<unsigned int CHANNELS>
struct IApp : public IBaseApp {
	virtual void init(void) = 0;
	virtual void update(void) = 0;

	virtual void setup(void) {
		memset( Values, 0, sizeof(Values) );
		Graph.setup(200);
		init();
	}

	virtual void refresh(void) final {
		Graph.update( Values );
	}

	virtual void draw(void) final {
		Graph.draw();
	}

	static const int     Vars = CHANNELS;
	OGLGraph<CHANNELS>   Graph;
	double               Values[CHANNELS];
};

template<unsigned int CHANNELS>
unsigned char OGLGraph<CHANNELS>::LineConfigS::Counter = 0;
