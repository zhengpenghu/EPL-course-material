/*
 * Source.cpp
 *
 *  Created on: May 5, 2015
 *      Author: chase
 */


#include <iostream>
#include <cstdint>
#include <type_traits>



using std::cout;
using std::endl;



void doit(void) { cout << "Hello\n"; }

template <typename T> struct abstract_fun;
template <typename Ret, typename... Args> struct abstract_fun<Ret(Args...)> {
	virtual Ret operator()(Args...) = 0;
	virtual ~abstract_fun(void) {}
	virtual abstract_fun<Ret(Args...)>* copy(void) const = 0;
};

//general function
template <typename T> struct fun;
template <typename Ret, typename... Args>
struct fun<Ret(Args...)> : public abstract_fun<Ret(Args...)> {
	using Same = fun<Ret(Args...)>;
	using PtrType = Ret (*)(Args...);

	PtrType ptr;

	Same* copy(void) const { return new Same{*this}; }
	fun(PtrType ptr) { this->ptr = ptr; }

	Ret operator()(Args... args) {
		return (*ptr)(args...);
	}
};


//member function, one more arguments which is T the class name
template <typename ObjType, typename FunType> struct mem_fun;
template <typename T, typename Ret, typename... Args>
struct mem_fun <T, Ret(Args...)> : public abstract_fun<Ret(T&, Args...)> {
	using Same = mem_fun<T, Ret(Args...)>;
	using PtrType = Ret (T::*)(Args...);

	PtrType ptr;

	Same* copy(void) const { return new Same{*this}; }

	mem_fun(PtrType ptr) { this->ptr = ptr; }

	Ret operator()(T& obj, Args... args) {
		return (obj.*ptr)(args...);
	}
};


/* in class I didn't have time to go through the third subtype of function -- function objects
 * Keep in mind that we're trying to create a single type, gen_fun, that allows a single variable
 * instance, e.g., f to be assigned different functions. I should be able to assign to f
 * a pointer-to-function (backwards compatibility to C), a pointer-to-member function (provided we
 * account for the implicit argument), and an C++ style function object -- i.e., a class with an operator()
 *
 * The function objects are slightly problematic, as there can be an infinite number of different classes
 * struct Foo { Ret operator()(Args...) { some code } };
 * We want to allow our gen_fun variable, f to be assigned an instance of Foo and/or assign f
 * and instance of Foo2, or Bar, or any other type that has an operator() with the correct return type
 * and correct argument types.
 *
 * The actual container type -- the subclass of abstract_fun that we need -- is really easy
 * BUT, we need to ensure that our FunObj actually has an operator() with the correct return and
 * argument types. I'm going to try and ignore that problem in the struct, but solve the problem
 * in the factory method for gen_fun
 *
 * i.e., I'll define the obj_fun subclass of abstract_fun where it just assumes that there's an
 * operator() with the correct argument/return type. Then, I'll try to make sure I don't create
 * any obj_fun objects unless there really is the right kind of operator()
 */

template <typename FunObj, typename Ret, typename... Args>
struct fun_obj : public abstract_fun<Ret(Args...)> {
	using Same = fun_obj<FunObj, Ret, Args...>;
	FunObj fun;

	fun_obj(const FunObj& f) : fun(f) {}

	Ret operator()(Args... args) { return fun(args...); }

	Same* copy(void) const { return new Same{*this}; }
};


struct LifeForm {
	void doit(void) { cout << "Hello from Lifeform\n"; }
	static void sdoit(void) { cout << "Static LifeForm\n"; }
};

void doit2(LifeForm& x) {
	cout << "doit2 says hello\n";
}

template <typename T1, typename T2>
constexpr bool IsSame(void) { return std::is_same<T1, T2>::value; }
template <bool p, typename T> using EnableIf = typename std::enable_if<p, T>::type;



template <typename T> struct gen_fun;

template <typename Ret, typename... Args> struct gen_fun<Ret(Args...)> {
private:
    //有一个abstract_fun的function pointer ,可以接受三类function 的pointer
	abstract_fun<Ret(Args...)>* ptr;
	using Same = gen_fun<Ret(Args...)>;

public:
	gen_fun(Same const & that) { ptr = that.ptr->copy(); } // copy constructor
	Same& operator=(Same const & that) { // copy assignment
		if (this != &that) {
			delete ptr;
			ptr = that.ptr->copy();
		}
		return *this;
	}
	~gen_fun(void) { delete ptr; }
	gen_fun(void) { ptr = nullptr; }

	Ret operator()(Args... args) {
		return (*ptr)(args...);
	}

	/* FACTORY METHODS */
    //总共提供了针对三种不同function的gen_fun，每种提供了assignment和constructor 共6个函数
	/* first one is easy..., pointer to ordinary function,
	 * I have both an assignment operator and a constructor for pointer-to-function */
	Same& operator=(Ret (*p)(Args...)) {
		delete ptr;
		ptr = new fun<Ret(Args...)>{p};
		return *this;
	}

	gen_fun(Ret (*p)(Args...)) { ptr = new fun<Ret(Args...)>{p}; }

	/* the second one is harder, pointer-to-member function
	 * in this case, we have a member function of the type Ret (Obj::*)(SomeArgs...)
	 * When that function is invoked, we need an Obj& reference -- the actual object
	 * to which the member function is being applied. For example, if we had a LifeForm
	 * member function, like border_cross, which is void (LifeForm::*)(void)
	 * and we want to call that function, we need a LifeForm object --> x.border_cross()
	 * In this design, we are mandating that our function object (i.e., the *this function
	 * object) take this Obj& reference as the first argument in its argument list.
	 *
	 * So.. if we have a pointer-to-member-function Ret (Obj::*)(SomeArgs...) then
	 * that is OK, only if our function type Ret(Args...) is the same as Ret(Obj&, SomeArgs...)
	 *
	 * I'll use an enable_if to hide this function when the argument lists don't match.
	 * As a simple trick, I'm going to compare the argument lists by packing them into a
	 * std::tuple<...> type and then compare the two tuple types -- i.e., I'm not going to
	 * go through the trouble of writing a big complex meta function that compares variadic
	 * lists of type arguments -- I'm just going to use std::is_same to compare two types
	 * std::tuple<Args...> and std::tuple<Obj&, SomeArgs...>
	 *
	 *
	 * In the code that follows, I have an assignment operator, and a constructor defined to implement
	 * this variation of the factory method
	 */
	template <typename Obj, typename... Args2>
	EnableIf<IsSame<std::tuple<Args...>, std::tuple<Obj&,Args2...>>(), Same&> // return type, Same& if enabled
	operator=(Ret (Obj::*p)(Args2...)) {
		delete ptr;
		ptr = new mem_fun<Obj, Ret(Args2...)>{p};
		return *this;
	}

	template <typename Obj, typename... Args2
	, typename=EnableIf<IsSame<std::tuple<Args...>, std::tuple<Obj&,Args2...>>(), int> // extra (unused) template arg, int if enabled
	>
	gen_fun(Ret (Obj::*p)(Args2...)){
		ptr = new mem_fun<Obj, Ret(Args2...)>(p);
	}


	/* OK, now for the final factory method. This one grabs all the function objects, including lambdas
	 * The only challenge here is disabling these methods when the actual function object is missing the
	 * correct type of operator(). I'm going to use decltype on the FunObj::operator() method
	 *
	 * I'm going to use decltype on an expression that tries to compute the address of the FunObj::operator()
	 * That decltype expression can evaluate only if there actually is an operator(), and we'll get a substitution
	 * failure error otherwise -- i.e., we confirm that FunObj really is a function object
	 *
	 * There's a couple of subtleties here that (quite fortunately) get resolved simultaneously by using
	 * a single type cast.
	 * Subtlety #1 -- what if FunObj has two (or more) overloaded operator() functions?
	 *    In this case, we only care about the one overload that matches our declared function type Ret(Args...)
	 *    and if there are other overloads, that's fine -- we'll ignore them. However, when a member function
	 *    is overloaded, and you try to take the address of that member function, you run into trouble. This is
	 *    annoying, and I think it's because of the complexities of parsing expressions in C++, but the remedy
	 *    is to use a type cast. So
	 *    		&FunObj::operator()(Args...) -- does not compile
	 *    		&FunObj::operator() -- is potentially ambiguous, if operator() is overloaded
	 *
	 *    		(Ret (FunObj::*)(Args...)) & FunObj::operator() -- compiles, thanks to the type cast
	 * Subtlety #2 -- we need to confirm that FunObj::operator() has the correct return type and the correct
	 * argument types. Well, thanks to the type cast we already needed (because of Subtlety #1), we've already
	 * confirmed the correct types -- that type cast can succeed only if there is an operator() with the right type
	 */

	template <typename FunObj,
		typename=decltype((Ret (FunObj::*)(Args...)) &FunObj::operator()())>
	Same& operator=(FunObj const& f) {
		delete ptr;
		ptr = new fun_obj<FunObj, Ret, Args...>{f};
		return *this;
	}

	template <typename FunObj,
		typename=decltype((Ret (FunObj::*)(Args...)) &FunObj::operator()())>
	gen_fun(FunObj const& f) {
		ptr = new fun_obj<FunObj, Ret, Args...>{f};
	}

};

int main(void) {
	fun<void(void)> p = &doit;

	p();

	p = &LifeForm::sdoit;
	p();

	mem_fun<LifeForm, void(void)> q = &LifeForm::doit;
	LifeForm fred;
	q(fred);

	fun<void(LifeForm&)> q2 = &doit2;
	q2(fred);

	gen_fun<void(LifeForm&)> fun(&doit2);
	fun(fred);
	fun = &LifeForm::doit;
	fun(fred);

	fun = [] (LifeForm& x) { x.doit(); };
	fun(fred);
}


#ifdef notdef

template <typename T> struct abstract_fun;
template <typename Ret, typename... Args> struct abstract_fun<Ret(Args...)> {
	virtual Ret operator()(Args...) = 0;
	virtual ~abstract_fun(void) {}
	virtual abstract_fun* copy(void) const = 0;
};

template <typename T> struct ptr_fun;
template <typename Ret, typename... Args>
struct ptr_fun<Ret(Args...)> : public abstract_fun<Ret(Args...)> {
	using Same = ptr_fun<Ret(Args...)>;
	using PtrType = Ret (*)(Args...);
	PtrType ptr;
	ptr_fun(PtrType ptr) { this->ptr = ptr; }
	Ret operator()(Args... args) {
		return (*ptr)(args...);
	}

	Same* copy(void) const { return new Same(*this); }
};

void doit(void) { cout << "Hello\n"; }

//class LifeForm {
//public:
//	void doit(void) { cout << "LifeForm\n"; }
//	static void sdoit(void) { cout << "static LifeForm\n"; }
//
//};

template <typename ObjType, typename Fun> struct ptr_mem_fun;

template <typename T, typename Ret, typename... Args>
struct ptr_mem_fun<T,Ret(Args...)> : public abstract_fun<Ret(T&, Args...)>{
	using Same = ptr_mem_fun<T, Ret(Args...)>;
	using PtrType = Ret (T::*)(Args...);
	PtrType ptr;
	ptr_mem_fun(PtrType ptr) { this->ptr = ptr; }

	Ret operator()(T& obj, Args... args) {
		return (obj.*ptr)(args...);
	}
	Same* copy(void) const { return new Same(*this); }
};

template <typename T> struct function;
template <typename Ret, typename... Args> class function<Ret(Args...)> {
private:
	abstract_fun<Ret(Args...)>* ptr = nullptr;
public:
	function(void) {}
	function(abstract_fun<Ret(Args...)> const & f) {
		ptr = f.copy();
	}
	~function(void) { delete ptr; }

	function(function<Ret(Args...)> const& that) { ptr = that.ptr->copy(); }

	function<Ret(Args...)>& operator=(function<Ret(Args...)> const& that) {
		delete ptr;
		ptr = that.ptr->copy();
	}

	Ret operator()(Args... args) { return (*ptr)(args...); }
};


int main(void) {
    cout<<"hello world!!"<<endl;
	ptr_fun<void(void)> p = &doit;

	p();

	p = &LifeForm::sdoit;
	p();

	ptr_mem_fun<LifeForm, void(void)> q = &LifeForm::doit;
    std::cout<<"after q=lifefrom::doit"<<std::endl;
	LifeForm f;
	q(f);

	function<void(LifeForm&)> fun = q;
	fun(f);
}

#endif /* notdef */
