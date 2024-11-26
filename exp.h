#ifndef EXP_H
#define EXP_H
#include "imp_value.hh"
#include "imp_type.hh"
#include <string>
#include <unordered_map>
#include <list>
#include "visitor.h"
#include "type_visitor.h"
using namespace std;
enum BinaryOp
{
    PLUS_OP, MINUS_OP, MUL_OP, DIV_OP,LT_OP, LE_OP, GE_OP, GT_OP, EQ_OP,
};
class Body;
class ImpValueVisitor;
class VarInit;
class Exp {
public:
    virtual int  accept(Visitor* visitor) = 0;
    virtual ImpValue accept(ImpValueVisitor* v) = 0;
    virtual ImpType accept(TypeVisitor* v) = 0;
    virtual ~Exp() = 0;
    static string binopToChar(BinaryOp op);
};
class IFExp : public Exp {
public:
    Exp *cond,*left, *right;
    IFExp(Exp *cond, Exp* l, Exp* r);
    int accept(Visitor* visitor);
    ImpValue accept(ImpValueVisitor* v);
    ImpType accept(TypeVisitor* v);
    ~IFExp();
};


class BinaryExp : public Exp {
public:
    Exp *left, *right;
    BinaryOp op;
    BinaryExp(Exp* l, Exp* r, BinaryOp op);
    int accept(Visitor* visitor);
    ImpValue accept(ImpValueVisitor* v);
    ImpType accept(TypeVisitor* v);
    ~BinaryExp();
};

class NumberExp : public Exp {
public:
    int value;
    NumberExp(int v);
    int accept(Visitor* visitor);
    ImpValue accept(ImpValueVisitor* v);
    ImpType accept(TypeVisitor* v);
    ~NumberExp();
};

class BoolExp : public Exp {
public:
    int value;
    BoolExp(bool v);
    int accept(Visitor* visitor);
    ImpValue accept(ImpValueVisitor* v);
    ImpType accept(TypeVisitor* v);
    ~BoolExp();
};

class IdentifierExp : public Exp {
public:
    std::string name;
    IdentifierExp(const std::string& n);
    int accept(Visitor* visitor);
    ImpValue accept(ImpValueVisitor* v);
    ImpType accept(TypeVisitor* v);
    ~IdentifierExp();
};

enum UnaryOp { INCREMENT_OP , DECREMENT_OP};
class UnaryExp: public Exp{
public:
    Exp* operand;
    UnaryOp op;
    UnaryExp(Exp* operand, UnaryOp op);
    int accept(Visitor* visitor);
    ImpValue accept(ImpValueVisitor* v);
    ImpType accept(TypeVisitor* v);

    ~UnaryExp();
};





//-------------------------------------

class FCallExp : public Exp {
public:
    std::string fname;
    list<Exp*> args;
    FCallExp(std::string fname, list<Exp*> args);
    int accept(Visitor* visitor);
    ImpValue accept(ImpValueVisitor* v);
    ImpType accept(TypeVisitor* v);
    ~FCallExp();
};

class Stm {
public:
    virtual int accept(Visitor* visitor) = 0;
    virtual ~Stm() = 0;
    virtual void accept(ImpValueVisitor* v) = 0;
    virtual void accept(TypeVisitor* v) = 0;
};

class FCallStatement: public Stm {
public:
    std::string fname;
    list<Exp*> args;
    FCallStatement(std::string fname, list<Exp*> args);
    void accept(ImpValueVisitor* v);
    void accept(TypeVisitor* v);
    int accept(Visitor* visitor);
    ~FCallStatement();
};

class AssignStatement : public Stm {
public:
    std::string id;
    Exp* rhs;
    AssignStatement(std::string id, Exp* e);
    int accept(Visitor* visitor);
    void accept(ImpValueVisitor* v);
    void accept(TypeVisitor* v);
    ~AssignStatement();
};

class PrintStatement : public Stm {
public:
    Exp* e;
    string format;
    PrintStatement(Exp* e);
    PrintStatement(Exp* e, string format) : e(e), format(format) {}
    int accept(Visitor* visitor);
    void accept(ImpValueVisitor* v);
    void accept(TypeVisitor* v);
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
    void accept(TypeVisitor* v);
    ~IfStatement();
};
class WhileStatement : public Stm {
public:
    Exp* condition;
    Body* b;
    WhileStatement(Exp* condition, Body* b);
    int accept(Visitor* visitor);
    void accept(ImpValueVisitor* v);
    void accept(TypeVisitor* v);
    ~WhileStatement();
};

class ForStatement : public Stm {
public:
    VarDec* init;
    Exp* condition;
    Stm* increment;
    Body* body;
    ForStatement(VarDec* init, Exp* condition, Stm* increment, Body* body);
    int accept(Visitor* visitor);
    void accept(ImpValueVisitor* v);
    void accept(TypeVisitor* v);
    ~ForStatement();
};

class VarDec {
public:
    string type;
    list<VarInit*> varinits;
    VarDec(string type, list<VarInit*> varinits);
    int accept(Visitor* visitor);
    void accept(ImpValueVisitor* v);
    void accept(TypeVisitor* v);
    ~VarDec();
};

class VarDecList{
public:
    list<VarDec*> vardecs;
    VarDecList();
    void add(VarDec* vardec);
    int accept(Visitor* visitor);
    void accept(ImpValueVisitor* v);
    void accept(TypeVisitor* v);
    ~VarDecList();
};

class StatementList {
public:
    list<Stm*> stms;
    StatementList();
    void add(Stm* stm);
    int accept(Visitor* visitor);
    void accept(ImpValueVisitor* v);
    void accept(TypeVisitor* v);
    ~StatementList();
};


class Body{
public:
    VarDecList* vardecs;
    StatementList* slist;
    Body(VarDecList* vardecs, StatementList* stms);
    int accept(Visitor* visitor);
    void accept(ImpValueVisitor* v);
    void accept(TypeVisitor* v);
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
  void accept(TypeVisitor* v);
  ~FunDec();
};

class FunDecList {
public:
  list<FunDec*> flist;
  FunDecList();
  void add(FunDec* s);
  int accept(Visitor* v);
  void accept(ImpValueVisitor* v);
  void accept(TypeVisitor* v);
  ~FunDecList();
};

class ReturnStatement : public Stm {
public:
  Exp* e;
  ReturnStatement(Exp* e);
  int accept(Visitor* v);
  void accept(ImpValueVisitor* v);
  void accept(TypeVisitor* v);
  ~ReturnStatement();
};


class Program {
public:
    VarDecList* vardecs;
    FunDecList* fundecs;
    Program(VarDecList* vardecs, FunDecList* fundecs);
    int accept(Visitor* v);
    void accept(ImpValueVisitor* v);
    void accept(TypeVisitor* v);
    ~Program();
};

class VarInit {
public:
    std::string name;
    Exp* init; // Puede ser NULL si no hay inicialización
    VarInit(std::string name, Exp* init);
    ~VarInit();
};



#endif // EXP_H