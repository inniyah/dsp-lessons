#ifndef STATIC_CHECKS_H_
#define STATIC_CHECKS_H_

#include <cassert>
#include <cstdlib>

template<bool Condition> struct CompileTimeError;
template<> struct CompileTimeError<false> { };

#define STATIC_CHECK(msg, expr) { CompileTimeError<expr> ERROR_##msg; (void)ERROR_##msg; }

template<int ACTUAL_SIZE, int ALLOWED_SIZE>
struct AllowedSize {
	static void constraint() {
		STATIC_CHECK(AllowedSizeNotEnough, (ACTUAL_SIZE > ALLOWED_SIZE));
	}
	AllowedSize() {
		void (*p)() = constraint;
		(*p)();
	}
};

template<class T, class B>
struct DerivedFrom {
	static void constraints(T* p) {
		B* pb = p;
		pb = pb;
	}
	DerivedFrom() {
		void(*p)(T*) = constraints;
		p = p;
	}
};

template<class T1, class T2>
struct CanCopy {
	static void constraints(T1 a, T2 b) {
		T2 c = a;
		b = a;
		c = a;
		b = a;
	}
	CanCopy() {
		void(*p)(T1,T2) = constraints;
	}
};

template<class T1, class T2 = T1>
struct CanCompare {
	static void constraints(T1 a, T2 b) {
		a == b;
		a != b;
		a < b;
	}
	CanCompare() {
		void(*p)(T1,T2) = constraints;
	}
};

#endif // STATIC_CHECKS_H_
