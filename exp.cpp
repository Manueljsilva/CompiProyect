#include <iostream>
#include "exp.h"
#include "imp_value_visitor.hh"

using namespace std;
IFExp::IFExp(Exp* cond,Exp* l, Exp* r): cond(cond),left(l),right(r){}
BinaryExp::BinaryExp(Exp* l, Exp* r, BinaryOp op):left(l),right(r),op(op) {}
NumberExp::NumberExp(int v):value(v) {}
BoolExp::BoolExp(bool v):value(v) {}
IdentifierExp::IdentifierExp(const string& n):name(n) {}
FCallExp::FCallExp(string fname, list<Exp*> args):fname(fname),args(args) {}
Exp::~Exp() {}
BinaryExp::~BinaryExp() { delete left; delete right; }
IFExp::~IFExp() {delete cond, delete left; delete right; }
NumberExp::~NumberExp() { }
BoolExp::~BoolExp() { }
IdentifierExp::~IdentifierExp() { }
FCallExp::~FCallExp() { 
    while (!args.empty()) {
        delete args.front();
        args.pop_front();
    }
}
AssignStatement::AssignStatement(string id, Exp* e): id(id), rhs(e) {}
AssignStatement::~AssignStatement() {
    delete rhs;
}


IfStatement::IfStatement(Exp* c, Body* t, Body* e): condition(c), then(t), els(e) {}
IfStatement::~IfStatement() {
    delete condition;
    delete then;
    delete els;
}
WhileStatement::WhileStatement(Exp* c, Body* t): condition(c), b(t) {}
WhileStatement::~WhileStatement() {
    delete condition;
    delete b;
}
ForStatement::ForStatement(VarDec* init, Exp* condition, Stm* increment, Body* body)
    : init(init), condition(condition), increment(increment), body(body) {}
ForStatement::~ForStatement() {
    delete init;
    delete condition;
    delete increment;
    delete body;
}


VarDec::VarDec(string type, list<VarInit*> vars) : type(type), vars(vars) {}

VarDec::~VarDec() {
    for (auto varInit : vars) {
        delete varInit;
    }
}


VarDecList::VarDecList(): vardecs() {}
void VarDecList::add(VarDec* v) {
    vardecs.push_back(v);
}
VarDecList::~VarDecList() {
    for (auto v: vardecs) {
        delete v;
    }
}

StatementList::StatementList(): stms() {}
void StatementList::add(Stm* s) {
    stms.push_back(s);
}
StatementList::~StatementList() {
    for (auto s: stms) {
        delete s;
    }
}

Body::Body(VarDecList* v, StatementList* s): vardecs(v), slist(s) {}
Body::~Body() {
    delete vardecs;
    delete slist;
}

FunDec::FunDec(string fname, list<string> types, list<string> vars, string rtype, Body* b): fname(fname), types(types), vars(vars), rtype(rtype), body(b) {}
FunDec::~FunDec() {
    delete body;
}

FunDecList::FunDecList(): flist() {}
void FunDecList::add(FunDec* f) {
    flist.push_back(f);
}
FunDecList::~FunDecList() {
    for (auto f: flist) {
        delete f;
    }
}

ReturnStatement::ReturnStatement(Exp* e): e(e) {}
ReturnStatement::~ReturnStatement() {
    delete e;
}

Program::Program(VarDecList* v, FunDecList* f): vardecs(v), fundecs(f) {}

Program::~Program() {
    delete vardecs;
    delete fundecs;
}
Stm::~Stm() {}
string Exp::binopToChar(BinaryOp op) {
    string  c;
    switch(op) {
        case PLUS_OP: c = "+"; break;
        case MINUS_OP: c = "-"; break;
        case MUL_OP: c = "*"; break;
        case DIV_OP: c = "/"; break;
        case LT_OP: c = "<"; break;
        case LE_OP: c = "<="; break;
        case GE_OP: c = ">="; break;
        case GT_OP: c = ">"; break;
        case EQ_OP: c = "=="; break;
        default: c = "$";
    }
    return c;
}


VarInit::VarInit(std::string name, Exp* init) : name(name), init(init) {}

VarInit::~VarInit() {
    if (init != nullptr) {
        delete init;
    }
}


// exp.cpp

FCallStatement::FCallStatement(std::string fname, std::list<Exp*> args) : fname(fname), args(args) {}

FCallStatement::~FCallStatement() {
    for (auto arg : args) {
        delete arg;
    }
}

int FCallStatement::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

void FCallStatement::accept(ImpValueVisitor* v) {
    v->visit(this);
}


UnaryExp::UnaryExp(Exp* operand, UnaryOp op) : operand(operand), op(op) {}
UnaryExp::~UnaryExp() {
    delete operand;
}

int UnaryExp::accept(Visitor* visitor)  {
    visitor->visit(this);
    return 0;
}

ImpValue UnaryExp::accept(ImpValueVisitor* v)  {
    return v->visit(this);
}
