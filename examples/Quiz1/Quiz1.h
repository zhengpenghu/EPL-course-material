/*
 * Quiz1.h
 *
 *  Created on: Mar 11, 2015
 *      Author: chase
 */

#ifndef QUIZ1_H_
#define QUIZ1_H_

namespace epl_quiz1 {

template <typename Expr>
struct assignment;

struct IntExpr {
	int val;
	IntExpr(int v) : val{v} {}
	int operator()(void) { return val; }
};

std::ostream& operator<<(std::ostream& out, IntExpr e) {
	return out << e();
}

struct Var {
	int val;

	template <typename Expr>
	assignment<Expr> operator=(Expr e);

	assignment<IntExpr> operator=(int e);

	operator int(void) const { return val; }
};

template<typename Expr>
struct assignment {
	Var& var;
	Expr e;

	assignment(Var& v, Expr ex) : var{v}, e{ex} {}
	void operator()(void) {
		var.val = e();
	}
};

template <typename Expr>
assignment<Expr> Var::operator=(Expr e) {
	return assignment<Expr>{*this, e};
}

assignment<IntExpr> Var::operator=(int v) {
	return assignment<IntExpr>{*this, v};
}

template <typename Expr>
struct AddExpr {
	Var& v;
	Expr e;
	AddExpr(Var& _v, Expr const& _e) : v{_v}, e{_e} {}
	int operator()(void) { return v.val + e(); }
};

AddExpr<IntExpr> operator+(Var& v, int x) {
	return AddExpr<IntExpr>{v, IntExpr{x}};
}

struct VarExpr {
	Var& var;
	VarExpr(Var& v) : var(v) {}
	int operator()(void) { return var.val; }
};


AddExpr<VarExpr> operator+(Var& v, Var& v2) {
	return AddExpr<VarExpr>{v, VarExpr{v2}};
}

Var _X{}, _Y{}, _Z{};



template <typename T>
struct print {
	T x;
	print(T const& _x) : x{_x} {}
	void operator()(void) { cout << x << endl; }
};


template <typename T>
print<T> Print(const T& v) {
    
    cout<<"123"<<endl;
    return print<T>{v}; }
//
//inline
//print<Var&> Print(Var& v) { return print<Var&>{v}; }

print<char const*> Print(char const* v) { return print<char const*>{v}; }

template <typename... STMTS>
struct program;

template <typename STMT>
struct program<STMT> {
	STMT body;
	program(STMT s) : body{s} {}
	void operator()(void) { body(); }
};

template <typename STMT, typename... STMTS>
struct program<STMT, STMTS...> {
	STMT body;
	program<STMTS...> continuation;

	program(STMT s, STMTS... rest) : body{s}, continuation{rest...} {}
	
    void operator()(void) {
		body();
		continuation();
	}
};

template <typename... S>
program<S...> Program(const S... s) {
	return program<S...>(s...);
}


} // end namespace epl_quiz1

#endif /* QUIZ1_H_ */
