#ifndef FLAGS_H_
#define FLAGS_H_

#include <cstdio>

template<typename N, N MASK = ~0>
class Flags {
public:
	typedef N Type;
	static const unsigned int Bits = sizeof(N)*8;
	static const N            Mask = static_cast<N>(MASK);
	static const N            None = static_cast<N>(0) & Mask;
	static const N            All  = static_cast<N>(~0) & Mask;

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

	inline void setAll() {
		m_Data = All;
	}

	inline void resetAll() {
		m_Data = None;
	}

	inline void toggle(unsigned int bit) {
		m_Data ^= static_cast<N>(1 << bit);
	}

	inline bool isEmpty() {
		return (None == m_Data);
	}

	inline bool isFull() {
		return ((All & Mask) == (m_Data & Mask));
	}

	void printBits(bool ret = false) {
		N mask = Mask;
		N data = m_Data;
		printf("<");
		for (unsigned int i = 0; i < Bits; ++i) {
			if ((mask & 1) != 0) {
				if ((data & 1) != 0) {
					printf("1");
				} else {
					printf("0");
				}
			} else {
				if ((data & 1) != 0) {
					printf("?");
				} else {
					printf("-");
				}
			}
			mask >>= 1;
			data >>= 1;
			if (!mask) {
				break;
			}
		}
		if (ret) {
			printf(">\n");
		} else {
			printf(">");
		}
	}

private:
	N m_Data;
};

#endif // FLAGS_H_
