#include "expr.cpp"
//Définition récursive de tseytin1
/***** DEFINI *****/
Expr& EEt::tseytin1(int& lastvar){
    lastvar--;
    Expr& myvar = *new EVar(lastvar);
    EVar& varop1 = *new EVar(lastvar-1);
    Expr& Top1 = op1.tseytin1(lastvar);
    EVar& varop2 = *new EVar(lastvar-1);
    Expr& Top2 = op2.tseytin1(lastvar);
    EOu& clause1 = *new EOu(*new ENot(myvar),varop1);
    EOu& clause2 = *new EOu(*new ENot(myvar),varop2);
    EOu& clause3 = *new EOu(*new EOu(myvar,*new ENot(varop1)),*new ENot(varop2));
    EEt& selfmod = *new EEt(*new EEt(clause1,clause2),clause3);
    return *new EEt(*new EEt(Top1,Top2), selfmod);
}
/***** DEFINI *****/
Expr& EOu::tseytin1(int& lastvar){
    lastvar--;
    EVar& myvar = *new EVar(lastvar);
    EVar& varop1 = *new EVar(lastvar-1);
    Expr& Top1 = op1.tseytin1(lastvar);
    EVar& varop2 = *new EVar(lastvar-1);
    Expr& Top2 = op2.tseytin1(lastvar);
    EOu& clause1 = *new EOu(*new ENot(varop1),myvar);
    EOu& clause2 = *new EOu(*new ENot(varop2),myvar);
    EOu& clause3 = *new EOu(*new EOu(*new ENot(myvar),varop1),varop2);
    EEt& selfmod = *new EEt(*new EEt(clause1,clause2),clause3);
    return *new EEt(*new EEt(Top1,Top2), selfmod);
}
/***** DEFINI *****/
Expr& EVar::tseytin1(int& lastvar){
    lastvar--;
    EVar& myvar = *new EVar(lastvar);
    EVar& self = *new EVar(etiq);
    EEt& selfmod = *new EEt(*new EOu(*new ENot(myvar),self),*new EOu(myvar,*new ENot(self)));
    return selfmod;
}
/***** DEFINI *****/
Expr& EEqiv::tseytin1(int& lastvar){
    lastvar--;
    EVar& myvar = *new EVar(lastvar);
    EVar& varop1 = *new EVar(lastvar-1);
    Expr& Top1 = op1.tseytin1(lastvar);
    EVar& varop2 = *new EVar(lastvar-1);
    Expr& Top2 = op2.tseytin1(lastvar);
    EOu& clause1 = *new EOu(*new EOu(*new ENot(myvar),varop1),*new ENot(varop2));
    EOu& clause2 = *new EOu(*new EOu(*new ENot(myvar),*new ENot(varop1)),varop2);
    EOu& clause3 = *new EOu(*new EOu(myvar,varop1),varop2);
    EOu& clause4 = *new EOu(*new EOu(myvar,*new ENot(varop1)),*new ENot(varop2));
    EEt& selfmod = *new EEt(*new EEt(*new EEt(clause1, clause2), clause3), clause4);
    return *new EEt(*new EEt(Top1,Top2), selfmod);
}
/***** DEFINI *****/
Expr& EImp::tseytin1(int& lastvar){
    lastvar--;
    EVar& myvar = *new EVar(lastvar);
    EVar& varop1 = *new EVar(lastvar-1);
    Expr& Top1 = op1.tseytin1(lastvar);
    EVar& varop2 = *new EVar(lastvar-1);
    Expr& Top2 = op2.tseytin1(lastvar);
    EOu& clause1 = *new EOu(myvar,varop1);
    EOu& clause2 = *new EOu(myvar,*new ENot(varop2));
    EOu& clause3 = *new EOu(*new EOu(*new ENot(myvar),*new ENot(varop1)),varop2);
    EEt& selfmod = *new EEt(*new EEt(clause1,clause2),clause3);
    return *new EEt(*new EEt(Top1,Top2), selfmod);
}
/***** DEFINI *****/
Expr& EXor::tseytin1(int& lastvar){
    lastvar--;
    EVar& myvar = *new EVar(lastvar);
    EVar& varop1 = *new EVar(lastvar-1);
    Expr& Top1 = op1.tseytin1(lastvar);
    EVar& varop2 = *new EVar(lastvar-1);
    Expr& Top2 = op2.tseytin1(lastvar);
    EOu& clause1 = *new EOu(*new EOu(myvar,varop1),*new ENot(varop2));
    EOu& clause2 = *new EOu(*new EOu(myvar,*new ENot(varop1)),varop2);
    EOu& clause3 = *new EOu(*new EOu(*new ENot(myvar),varop1),varop2);
    EOu& clause4 = *new EOu(*new EOu(*new ENot(myvar),*new ENot(varop1)),*new ENot(varop2));
    EEt& selfmod = *new EEt(*new EEt(*new EEt(clause1, clause2), clause3), clause4);
    return *new EEt(*new EEt(Top1,Top2), selfmod);
}
/***** DEFINI *****/
Expr& ENot::tseytin1(int& lastvar){
    lastvar--;
    EVar& myvar = *new EVar(lastvar);
    EVar& varop = *new EVar(lastvar-1);
    Expr& Top = op.tseytin1(lastvar);
    EEt& selfmod = *new EEt(*new EOu(*new ENot(myvar),*new ENot(varop)),*new EOu(myvar,varop));
    return *new EEt(Top, selfmod);
}
/***** DEFINI *****/
Expr& EConst::tseytin1(int& lastvar){
    lastvar--;
    EVar& myvar = *new EVar(lastvar);
    EConst& self = *new EConst(value);
    EEt& selfmod = *new EEt(*new EOu(*new ENot(myvar),self),*new EOu(myvar,*new ENot(self)));
    return selfmod;
}
Expr& Expr::tseytin(){
    int lastvar = 0;
    return *new EEt(*new EVar(lastvar), this->tseytin1(lastvar));
}
int main(){
    EVar foo = EVar(1);//EEt(*new EVar(1),*new EVar(2));
    cout << foo.to_string() << endl;
    int n=0;
    Expr& foo2=foo.tseytin();
    cout << foo2.to_string() << endl;
    return 0;
}
