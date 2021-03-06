#ifndef DELEGATE_H_
#define DELEGATE_H_

#include <cstdlib>

// See: http://blog.coldflake.com/posts/C++-delegates-on-steroids/

#if __cplusplus <= 199711L // not a C++11 compliant compiler
#error At least a C++11 compliant compiler is needed
#endif

// Usage:
//     A a;
//     auto d = Delegate<int, int>::from_function<A, &A::foo>(&a);
//     auto d2 = Delegate<int, int, int, char>::from_function<A, &A::bar>(&a);

template<typename return_type, typename... params>
class Delegate {
public:
	typedef return_type (*Type)(void* callee, params...);

	Delegate(void * callee, Type function) : fpCallee(callee) , fpCallbackFunction(function) { }
	Delegate(const Delegate & d) : fpCallee(d.fpCallee) , fpCallbackFunction(d.fpCallbackFunction) { }

	return_type operator()(params... xs) const {
		return (*fpCallbackFunction)(fpCallee, xs...);
	}

	Delegate & operator=(const Delegate & d) {
		fpCallee = d.fpCallee;
		fpCallbackFunction = d.fpCallbackFunction;
		return *this;
	}

	bool operator==(const Delegate & d) const {
		return ((fpCallee == d.fpCallee) && (fpCallbackFunction == d.fpCallbackFunction));
	}

	template <class T, return_type (T::*TMethod)(params...)>
	static Delegate fromObjectMethod(T * callee) {
		Delegate d(callee, &methodCaller<T, TMethod>);
		return d;
	}

private:
	void * fpCallee;
	Type fpCallbackFunction;

	template <class T, return_type (T::*TMethod)(params...)>
	static return_type methodCaller(void* callee, params... xs) {
		T * p = static_cast<T*>(callee);
		return (p->*TMethod)(xs...);
	}
};

#endif // DELEGATE_H_
