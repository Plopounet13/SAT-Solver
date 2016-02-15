#include <cstdio>
#include "expr.hpp"

using namespace std;

/***********************************/
/**********  Constants   ***********/
/***********************************/

EConst::EConst(bool val) : value(val) {}

string EConst::to_string()
{
    if (value)
		return "T";
	else
		return "F";
}

bool EConst::eval(map<int,int>& sigma)
{
    return value;
}

/***********************************/
/***********  Variables  ***********/
/***********************************/

EVar::EVar(int et) : etiq(et) {}

string EVar::to_string()
{
	stringstream ss;
	ss << etiq;
	return ss.str();
}

bool EVar::eval(map<int,int>& sigma)
{
    return sigma[etiq];
}

/***********************************/
/*********  Disjonction   **********/
/***********************************/

EOu::EOu(Expr * e1, Expr * e2) : op1(e1), op2(e2) {}

string EOu::to_string()
{
    return "(" + op1->to_string() + " \\/ " +  op2->to_string() + ")";
}

bool EOu::eval(map<int,int>& sigma)
{
    return op1->eval(sigma) || op2->eval(sigma);
}

/***********************************/
/**********  Conjonction   *********/
/***********************************/

EEt::EEt(Expr * e1, Expr * e2) : op1(e1), op2(e2) {}

string EEt::to_string()
{
    return "(" + op1->to_string() + " /\\ " +  op2->to_string() + ")";
}

bool EEt::eval(map<int,int>& sigma)
{
    return op1->eval(sigma) && op2->eval(sigma);
}

/***********************************/
/***********  Negation  ************/
/***********************************/

ENot::ENot(Expr * e) : op(e) {}

string ENot::to_string()
{
    return "(~" +op->to_string() + ")";
}

bool ENot::eval(map<int,int>& sigma)
{
    return !op->eval(sigma);
}

/***********************************/
/*****  Disjonction exclusive  *****/
/***********************************/

EXor::EXor(Expr * e1, Expr * e2) : op1(e1), op2(e2) {}

string EXor::to_string()
{
    return "(" + op1->to_string() + " X " +  op2->to_string() + ")";
}

bool EXor::eval(map<int,int>& sigma)
{
    return op1->eval(sigma) xor op2->eval(sigma);
}

/***********************************/
/*********  Impliquation  **********/
/***********************************/

EImp::EImp(Expr * e1, Expr * e2) : op1(e1), op2(e2) {}

string EImp::to_string()
{
    return "(" + op1->to_string() + " => " +  op2->to_string() + ")";
}

bool EImp::eval(map<int,int>& sigma)
{
    return !op1->eval(sigma) || op2->eval(sigma);
}


/***********************************/
/**********  Equivalence  **********/
/***********************************/

EEqiv::EEqiv(Expr * e1, Expr * e2) : op1(e1), op2(e2) {}

string EEqiv::to_string()
{
    return "(" + op1->to_string() + " <=> " +  op2->to_string() + ")";
}

bool EEqiv::eval(map<int,int>& sigma)
{
    return op1->eval(sigma) == op2->eval(sigma);
}

