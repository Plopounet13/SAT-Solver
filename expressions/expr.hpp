#ifndef DEF_EXPR
#define DEF_EXPR

#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <set>
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 1
#endif
using namespace std;
/***********************************/
/*********  Expressions   **********/
/***********************************/

class Expr
{
public:
    Expr(){}
    virtual string to_string()=0;
    virtual bool eval(map<int,int>& sigma)=0;
};

/***********************************/
/***********  Constants  ***********/
/***********************************/

class EConst : public Expr
{
public:
    EConst(bool val);
    virtual string to_string();
    virtual bool eval(map<int,int>& sigma);
private:
    bool value;
};


/***********************************/
/***********  Variables  ***********/
/***********************************/

class EVar : public Expr
{
public:
    EVar(int et);
    virtual string to_string();
    virtual bool eval(map<int,int>& sigma);
private:
    int etiq;
};

/***********************************/
/*********  Disjonction   **********/
/***********************************/

class EOu : public Expr
{
public:
    EOu(Expr& e1, Expr& e2);
    virtual string to_string();
	virtual bool eval(map<int,int>& sigma);
private:
    Expr& op1,& op2;
};

/***********************************/
/**********  Conjonction  **********/
/***********************************/

class EEt : public Expr
{
public:
    EEt(Expr& e1, Expr& e2);
    virtual string to_string();
	virtual bool eval(map<int,int>& sigma);
private:
    Expr& op1,& op2;
};

/***********************************/
/***********  Negation  ************/
/***********************************/

class ENot : public Expr
{
public:
    ENot(Expr& e);
    virtual string to_string();
	virtual bool eval(map<int,int>& sigma);
private:
    Expr& op;
};

/***********************************/
/*****  Disjonction exclusive  *****/
/***********************************/

class EXor : public Expr
{
public:
    EXor(Expr& e1, Expr& e2);
    virtual string to_string();
    virtual bool eval(map<int,int>& sigma);
private:
    Expr& op1,& op2;
};

/***********************************/
/*********  Impliquation  **********/
/***********************************/

class EImp : public Expr
{
public:
    EImp(Expr& e1, Expr& e2);
    virtual string to_string();
    virtual bool eval(map<int,int>& sigma);
private:
    Expr& op1,& op2;
};


/***********************************/
/**********  Equivalence  **********/
/***********************************/

class EEqiv : public Expr
{
public:
    EEqiv(Expr& e1, Expr& e2);
    virtual string to_string();
    virtual bool eval(map<int,int>& sigma);
private:
    Expr& op1,& op2;
};

#endif // ! DEF_EXPR
