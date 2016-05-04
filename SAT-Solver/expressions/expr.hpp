#ifndef DEF_EXPR
#define DEF_EXPR

#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <unordered_set>
#include <set>
#include <functional>

using namespace std;
/***********************************/
/*********  Expressions   **********/
/***********************************/


class Expr
{
public:
    Expr(){}
    ~Expr(){}
    virtual string to_string() const=0;
    virtual bool eval(map<int,int>& sigma)=0;
    Expr& tseytin(int lastvar);
	virtual void toEns(vector<unordered_set<int>>* v, int negation)=0;
    virtual Expr& tseytin1(int& lastvar)=0;
};

bool operator==(Expr& a, Expr& b);
bool operator <(const Expr& e1, const Expr& e2);

//toEns
vector<unordered_set<int>>* toEns(Expr& e);

/***********************************/
/***********  Constants  ***********/
/***********************************/

class EConst : public Expr
{
public:
    EConst(bool val);
    virtual string to_string() const;
    virtual bool eval(map<int,int>& sigma);
	virtual bool getValue();
	virtual Expr& tseytin1(int& lastvar);
	void toEns(vector<unordered_set<int>>* v, int negation);
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
    virtual string to_string() const;
	virtual bool eval(map<int,int>& sigma);
	virtual int getEtiq();
	virtual Expr& tseytin1(int& lastvar);
	void toEns(vector<unordered_set<int>>* v, int negation);
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
    virtual string to_string() const;
	virtual bool eval(map<int,int>& sigma);
	virtual Expr& getOp1();
	virtual Expr& getOp2();
	virtual Expr& tseytin1(int& lastvar);
	void toEns(vector<unordered_set<int>>* v, int negation);
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
    virtual string to_string() const;
	virtual bool eval(map<int,int>& sigma);
	virtual Expr& getOp1();
	virtual Expr& getOp2();
	virtual Expr& tseytin1(int& lastvar);
	void toEns(vector<unordered_set<int>>* v, int negation);
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
    virtual string to_string() const;
	virtual bool eval(map<int,int>& sigma);
	virtual Expr& getOp();
	virtual Expr& tseytin1(int& lastvar);
	void toEns(vector<unordered_set<int>>* v, int negation);
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
    virtual string to_string() const;
	virtual bool eval(map<int,int>& sigma);
	virtual Expr& getOp1();
	virtual Expr& getOp2();
	virtual Expr& tseytin1(int& lastvar);
	void toEns(vector<unordered_set<int>>* v, int negation);
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
    virtual string to_string() const;
	virtual bool eval(map<int,int>& sigma);
	virtual Expr& getOp1();
	virtual Expr& getOp2();
	virtual Expr& tseytin1(int& lastvar);
	void toEns(vector<unordered_set<int>>* v, int negation);
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
    virtual string to_string() const;
	virtual bool eval(map<int,int>& sigma);
	virtual Expr& getOp1();
	virtual Expr& getOp2();
	virtual Expr& tseytin1(int& lastvar);
	void toEns(vector<unordered_set<int>>* v, int negation);
private:
    Expr& op1,& op2;
};

#endif // ! DEF_EXPR
