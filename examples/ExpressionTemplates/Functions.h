/*
 * Functions.h
 *
 *  Created on: Mar 10, 2015
 *      Author: chase
 */

#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

struct IdentityFunction {
	double operator()(double x) const { return x; }
};


template <int ArgPos = 0, typename... ArgTypes> struct select_arg;

template <typename FirstArg, typename... OtherArgs>
struct select_arg<0, FirstArg, OtherArgs...> {
	using type = FirstArg;

	static FirstArg select(FirstArg x, OtherArgs...) { return x; }
};

template <int ArgPos, typename FirstArg, typename... OtherArgs>
struct select_arg<ArgPos, FirstArg, OtherArgs...> {
	using type = typename select_arg<ArgPos-1, OtherArgs...>::type;
	static type select(FirstArg, OtherArgs... args) {
		return select_arg<ArgPos-1, OtherArgs...>::select(args...);
	}
};


template <int argPos, typename... Args>
typename select_arg<argPos, Args...>::type selectArg(Args... args) {
	return select_arg<argPos, Args...>::select(args...);
}
template <typename F, int arg=0>
struct ArgWrapper : public F {
	static constexpr int argPos = arg;
	template <typename... Args>
	double operator()(Args... args) const {
        cout<<"argwrapper operator()"<<endl;
		return F::operator()(selectArg<argPos>(args...));
	}
};

//three instance
ArgWrapper<IdentityFunction, 0> _1{};
ArgWrapper<IdentityFunction, 1> _2{};
ArgWrapper<IdentityFunction, 2> _3{};


template <typename F, typename G>
struct AddProxy {
	F f;
	G g;
	double operator()(double x) { return f(x) + g(x); }
	double operator()(double x, double y) {
		return f(selectArg<F::argPos>(x, y)) + g(selectArg<G::argPos>(x, y));
	}
};

template <typename F, typename G>
struct SubProxy {
	F f;
	G g;
	double operator()(double x) { return f(x) + g(x); }

	template <typename... Args>
	double operator()(Args... args) {
        cout<<"in the subproxy operator()"<<endl;
		return f(args...) - g(args...);
	}
};

template <typename F, typename G>
struct MulProxy {
	F f;
	G g;
	double operator()(double x) { return f(x) * g(x); }
};

template <typename F, typename G>
AddProxy<F, G> operator+(const F& f, const G& g) {
    cout<<"addproxy"<<endl;
    //Is f and g function object?
    //cout<<F;
	return AddProxy<F, G>{f, g};
}

template <typename F, typename G>
SubProxy<F, G> operator-(const F& f, const G& g) {
    //cout<<"subproxy operator-"<<endl;
	return SubProxy<F, G>{f, g};//Is that default constructor from f,g to F,G????
}





#endif /* FUNCTIONS_H_ */
