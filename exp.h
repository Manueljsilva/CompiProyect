#ifndef EXP_H
#define EXP_H
#include "imp_value.hh"
#include "imp_type.hh"
#include <string>
#include <unordered_map>
#include <list>
#include "visitor.h"
#include "imp_value_visitor.hh"
using namespace std;
enum BinaryOp
{
    PLUS_OP, MINUS_OP, MUL_OP, DIV_OP,LT_OP, LE_OP, EQ_OP
};

class Body;
class ImpValueVisitor;
class Exp {
public:
    virtual int  accept(Visitor* visitor) = 0;
    virtual ImpValue accept(ImpValueVisitor* v) = 0;
    virtual ~Exp() = 0;
    static string binopToChar(BinaryOp op);
};
class IFExp : public Exp {
public:
    Exp *cond,*left, *right;
    IFExp(Exp *cond, Exp* l, Exp* r);
    int accept(Visitor* visitor);
    ImpValue accept(ImpValueVisitor* v);
    ~IFExp();
};


class BinaryExp : public Exp {
public:
    Exp *left, *right;
    BinaryOp op;
    BinaryExp(Exp* l, Exp* r, BinaryOp op);
    int accept(Visitor* visitor);
    ImpValue accept(ImpValueVisitor* v);
    ~BinaryExp();
};

class NumberExp : public Exp {
public:
    int value;
    NumberExp(int v);
    int accept(Visitor* visitor);
    ImpValue accept(ImpValueVisitor* v);
    ~NumberExp();
};

class BoolExp : public Exp {
public:
    int value;
    BoolExp(bool v);
    int accept(Visitor* visitor);
    ImpValue accept(ImpValueVisitor* v);
    ~BoolExp();
};

class IdentifierExp : public Exp {
public:
    std::string name;
    IdentifierExp(const std::string& n);
    int accept(Visitor* visitor);
    ImpValue accept(ImpValueVisitor* v);
    ~IdentifierExp();
};

class FCallExp : public Exp {
public:
    std::string fname;
    list<Exp*> args;
    FCallExp(std::string fname, list<Exp*> args);
    int accept(Visitor* visitor);
    ImpValue accept(ImpValueVisitor* v);
    ~FCallExp();
};

class Stm {
public:
    virtual int accept(Visitor* visitor) = 0;
    virtual ~Stm() = 0;
    virtual void accept(ImpValueVisitor* v) = 0;
};

class AssignStatement : public Stm {
public:
    std::string id;
    Exp* rhs;
    AssignStatement(std::string id, Exp* e);
    int accept(Visitor* visitor);
    void accept(ImpValueVisitor* v);
    ~AssignStatement();
};

class PrintStatement : public Stm {
public:
    Exp* e;
    PrintStatement(Exp* e);
    int accept(Visitor* visitor);
    void accept(ImpValueVisitor* v);
    ~PrintStatement();
};


class IfStatement : public Stm {
public:
    Exp* condition;
    Body* then;
    Body* els;
    IfStatement(Exp* condition, Body* then, Body* els);
    int accept(Visitor* visitor);
    void accept(ImpValueVisitor* v);
    ~IfStatement();
};
class WhileStatement : public Stm {
public:
    Exp* condition;
    Body* b;
    WhileStatement(Exp* condition, Body* b);
    int accept(Visitor* visitor);
    void accept(ImpValueVisitor* v);
    ~WhileStatement();
};

class ForStatement : public Stm {
public:
    VarDec* init; // initialization statement
    Exp* condition; // condition expression
    Stm* increment; // increment statement
    Body* body;
    ForStatement(VarDec* init, Exp* condition, Stm* increment, Body* body);
    int accept(Visitor* visitor);
    void accept(ImpValueVisitor* v);
    ~ForStatement();
};
class VarInit {
public:
    std::string name;
    Exp* init; // Puede ser NULL si no hay inicialización
    VarInit(std::string name, Exp* init);
    ~VarInit();
};

class VarDec {
public:
    std::string type;
    std::list<VarInit*> vars;
    VarDec(std::string type, std::list<VarInit*> vars);
    int accept(Visitor* visitor);
    void accept(ImpValueVisitor* v);
    ~VarDec();
};

class VarDecList{
public:
    list<VarDec*> vardecs;
    VarDecList();
    void add(VarDec* vardec);
    int accept(Visitor* visitor);
    void accept(ImpValueVisitor* v);
    ~VarDecList();
};

class StatementList {
public:
    list<Stm*> stms;
    StatementList();
    void add(Stm* stm);
    int accept(Visitor* visitor);
    void accept(ImpValueVisitor* v);
    ~StatementList();
};


class Body{
public:
    VarDecList* vardecs;
    StatementList* slist;
    Body(VarDecList* vardecs, StatementList* stms);
    int accept(Visitor* visitor);
    void accept(ImpValueVisitor* v);
    ~Body();
};

class FunDec {
public:
  string fname, rtype;
  list<string> vars;
  list<string> types;
  Body* body;
  FunDec(string fname, list<string> types, list<string> vars, string rtype, Body* body);
  int accept(Visitor* v);
  void accept(ImpValueVisitor* v);
  ~FunDec();
};

class FunDecList {
public:
  list<FunDec*> flist;
  FunDecList();
  void add(FunDec* s);
  int accept(Visitor* v);
  void accept(ImpValueVisitor* v);
  ~FunDecList();
};

class ReturnStatement : public Stm {
public:
  Exp* e;
  ReturnStatement(Exp* e);
  int accept(Visitor* v);
  void accept(ImpValueVisitor* v);
  ~ReturnStatement();
};

class Program {
public:
    VarDecList* vardecs;
    FunDecList* fundecs;
    Program(VarDecList* vardecs, FunDecList* fundecs);
    int accept(Visitor* v);
    void accept(ImpValueVisitor* v);
    ~Program();
};

class FCallStatement : public Stm {
public:
    std::string fname;
    std::list<Exp*> args;
    FCallStatement(std::string fname, std::list<Exp*> args);
    int accept(Visitor* visitor);
    void accept(ImpValueVisitor* v);
    ~FCallStatement();
};
enum UnaryOp { INCREMENT_OP , DECREMENT_OP};
class UnaryExp: public Exp{
public:
    Exp* operand;
    UnaryOp op;
    UnaryExp(Exp* operand, UnaryOp op);
    int accept(Visitor* visitor);
    ImpValue accept(ImpValueVisitor* v);
    ~UnaryExp();
};




#endif // EXP_H