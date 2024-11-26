#include <iostream>
#include "exp.h"
#include "visitor.h"
#include <unordered_map>
using namespace std;

///////////////////////////////////////////////////////////////////////////////////
int BinaryExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int NumberExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int BoolExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int IdentifierExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int AssignStatement::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

int FCallStatement::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

int PrintStatement::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

int IfStatement::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

int WhileStatement::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}
int ForStatement::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

int VarDec::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

int VarDecList::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}
int StatementList::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}
int Body::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

int Program::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}


int FunDec::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

int FunDecList::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

int ReturnStatement::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

int FCallExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

///////////////////////////////////////////////////////////////////////////////////

int PrintVisitor::visit(BinaryExp* exp) {
    exp->left->accept(this);
    cout << ' ' << Exp::binopToChar(exp->op) << ' ';
    exp->right->accept(this);
    return 0;
}

int PrintVisitor::visit(NumberExp* exp) {
    cout << exp->value;
    return 0;
}

int PrintVisitor::visit(BoolExp* exp) {
    if(exp->value) cout << "true";
    else cout << "false";
    return 0;
}

int PrintVisitor::visit(IdentifierExp* exp) {
    cout << exp->name;
    return 0;
}

void PrintVisitor::visit(AssignStatement* stm) {
    cout << stm->id << " = ";
    stm->rhs->accept(this);
    cout << ";";
}

void PrintVisitor::visit(PrintStatement* stm) {
    cout << "printf(";

    if (!stm->format.empty()) {
        cout << "\"" << stm->format << "\"";

        if (stm->e != nullptr) {
            cout << ", ";
            stm->e->accept(this);
        }
    } else if (stm->e != nullptr) {
        stm->e->accept(this);
    }

    cout << ");";
}

void PrintVisitor::visit(IfStatement* stm) {
    cout << "if (";
    stm->condition->accept(this);
    cout << ") {" << endl;

    stm->then->accept(this);

    if (stm->els) {
        printIndent();
        cout << "} else {" << endl;
        stm->els->accept(this);
    }

    printIndent();
    cout << "}";
}

void PrintVisitor::imprimir(Program* program){
    program->accept(this);
}

void PrintVisitor::visit(Program* program){
    program->vardecs->accept(this);
    program->fundecs->accept(this);
};


void PrintVisitor::visit(WhileStatement* stm){
    cout << "while ";
    stm->condition->accept(this);
    cout << " {" << endl;
    stm->b->accept(this);
    printIndent();
    cout << "}";
}

void PrintVisitor::visit(ForStatement* stm){
    cout << "for(";
    stm->init->accept(this);
    cout << ";";
    stm->condition->accept(this);
    cout << ";";
    stm->increment->accept(this);
    cout << "{" << endl;
    stm->body->accept(this);
    printIndent();
    cout << "}";

}

void PrintVisitor::visit(VarDec* stm) {
    for (auto varInit : stm->varinits) {
        cout << stm->type << " ";
        cout << varInit->name;
        if (varInit->init != nullptr) {
            cout << " = ";
            varInit->init->accept(this);
        }
    }
}

void PrintVisitor::visit(VarDecList* stm){
    for(auto i: stm->vardecs){
        printIndent();
        i->accept(this);
        cout << endl;
    }
}

void PrintVisitor::visit(StatementList* stm){
    for(auto i: stm->stms){
        printIndent();
        i->accept(this);
        cout << endl;
    }
}

void PrintVisitor::visit(Body* stm){
    increaseIndent();
    stm->vardecs->accept(this);
    stm->slist->accept(this);
    decreaseIndent();
}

void PrintVisitor::visit(FunDec* stm) {
    cout << stm->rtype << " " << stm->fname << "(";

    bool first = true;
    list<string>::iterator type, name;
    for (type = stm->types.begin(), name = stm->vars.begin();
         type != stm->types.end(); type++, name++) {
        if (!first) cout << ", ";
        cout << *type << " " << *name;
        first = false;
         }
    cout << ") {" << endl;

    increaseIndent();
    stm->body->accept(this);
    decreaseIndent();

    cout << "}" << endl;
}

void PrintVisitor::visit(FunDecList* stm){
    for(auto i: stm->flist){
        i->accept(this);
        cout << endl;
    }
}

void PrintVisitor::visit(ReturnStatement* s) {
    cout << endl;
    printIndent();
    cout << "return ";
    if (s->e != NULL) s->e->accept(this);
    cout << ";";
}

int PrintVisitor::visit(FCallExp* e) {
  cout << e->fname << "(";
  list<Exp*>::iterator it;
  bool first = true;
  for (it = e->args.begin(); it != e->args.end(); ++it) {
    if (!first) cout << ",";
    first = false;
    (*it)->accept(this);
  }
  cout << ')';
  return 0;
}

void PrintVisitor::visit(FCallStatement* e) {
  cout << e->fname << "(";
  list<Exp*>::iterator it;
  bool first = true;
  for (it = e->args.begin(); it != e->args.end(); ++it) {
    if (!first) cout << ",";
    first = false;
    (*it)->accept(this);
  }
  cout << ')';
  
}

void PrintVisitor::printIndent() {
    for (int i = 0; i < indentLevel*2; i++) {
    cout << ' ';
    }

}

///////////////////////////////////////////////////////////////////////////////////

int PrintVisitor::visit(UnaryExp* e) {
    e->operand->accept(this);
    if (e->op == INCREMENT_OP) {
        cout << "++";
    } else if (e->op == DECREMENT_OP) {
        cout << "--";
    }

    return 0;
}