#include <iostream>
#include <stdexcept>
#include "token.h"
#include "scanner.h"
#include "exp.h"
#include "parser.h"

using namespace std;

bool Parser::match(Token::Type ttype) {
    if (check(ttype)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::check(Token::Type ttype) {
    if (isAtEnd()) return false;
    return current->type == ttype;
}

bool Parser::advance() {
    if (!isAtEnd()) {
        Token* temp = current;
        if (previous) delete previous;
        current = scanner->nextToken();
        previous = temp;
        if (check(Token::ERR)) {
            cout << "Error de análisis, carácter no reconocido: " << current->text << endl;
            exit(1);
        }
        return true;
    }
    return false;
}

bool Parser::isAtEnd() {
    return (current->type == Token::END);
}

Parser::Parser(Scanner* sc):scanner(sc) {
    previous = NULL;
    current = scanner->nextToken();
    if (current->type == Token::ERR) {
        cout << "Error en el primer token: " << current->text << endl;
        exit(1);
    }
}


VarDec* Parser::parseVarDec() {
    VarDec* vd = NULL;
    if (match(Token::VAR)) {
        if (!match(Token::ID)) {
            cout << "Error: se esperaba un tipo después de 'var'." << endl;
            exit(1);
        }
        string type = previous->text;
        list<VarInit*> vars;

        if (!match(Token::ID)) {
            cout << "Error: se esperaba un identificador después del tipo." << endl;
            exit(1);
        }
        string varName = previous->text;
        Exp* initExp = NULL;

        if (match(Token::ASSIGN)) {
            initExp = parseCExp();
        }
        vars.push_back(new VarInit(varName, initExp));

        while (match(Token::COMA)) {
            if (!match(Token::ID)) {
                cout << "Error: se esperaba un identificador después de ','." << endl;
                exit(1);
            }
            varName = previous->text;
            initExp = NULL;

            if (match(Token::ASSIGN)) {
                initExp = parseCExp();
            }
            vars.push_back(new VarInit(varName, initExp));
        }

        if (!match(Token::PC)) {
            cout << "Error: se esperaba un ';' al final de la declaración." << endl;
            exit(1);
        }
        vd = new VarDec(type, vars);
    }
    return vd;
}


VarDecList* Parser::parseVarDecList() {
    VarDecList* vdl = new VarDecList();
    VarDec* aux;
    aux = parseVarDec();
    while (aux != NULL) {
        vdl->add(aux);
        aux = parseVarDec();
    }
    return vdl;
}

StatementList* Parser::parseStatementList() {
    StatementList* sl = new StatementList();
    sl->add(parseStatement());
    while (match(Token::PC)) {
        sl->add(parseStatement());
    }
    return sl;
}


Body* Parser::parseBody() {
    VarDecList* vdl = parseVarDecList();
    StatementList* sl = parseStatementList();
    return new Body(vdl, sl);
}

FunDec* Parser::parseFunDec() {
  FunDec* fd = NULL;
  if (match(Token::FUN)) {
    Body* body = NULL;
    if (!match(Token::ID)) {
        cout << "Error: se esperaba un tipo 'fun'." << endl;
        exit(1);
    }
    string rtype = previous->text;
    if (!match(Token::ID)) {
        cout << "Error: se esperaba un nombre de función después del tipo." << endl;
        exit(1);
    }
    string fname = previous->text;
    if (!match(Token::PI)) {
        cout << "Error: se esperaba un '(' después del nombre de la función." << endl;
        exit(1);
    }
    list<string> types;
    list<string> vars;
    if (!check(Token::PD)) {
      if (!match(Token::ID)) {
        cout << "Error: se esperaba un tipo después del '('." << endl;
        exit(1);
      }
      types.push_back(previous->text);
      if (!match(Token::ID)) {
        cout << "Error: se esperaba un identificador después del tipo." << endl;
        exit(1);
      }
      vars.push_back(previous->text);
      while(match(Token::COMA)) {
        if(!match(Token::ID)) {
          cout << "Error: se esperaba un tipo después de ','." << endl;
          exit(1);
        }
        types.push_back(previous->text);
        if (!match(Token::ID)) {
            cout << "Error: se esperaba un identificador después del tipo." << endl;
            exit(1);
        }
        vars.push_back(previous->text);
      }
    }
    if (!match(Token::PD)) {
        cout << "Error: se esperaba un ')' después de la lista de argumentos." << endl;
        exit(1);
    }
      if (!match(Token::LBRACE)) {
          cout << "Error: se esperaba '{' al inicio del cuerpo de la función." << endl;
          exit(1);
      }
    body = parseBody();
      // Cierre del cuerpo con '}'
      if (!match(Token::RBRACE)) {
          cout << "Error: se esperaba '}' al final del cuerpo de la función." << "pero se encontro: " << current->text << endl;
          exit(1);
      }
    fd = new FunDec(fname, types, vars, rtype, body);
  }
  return fd;
}

FunDecList* Parser::parseFunDecList() {
    FunDecList* fdl = new FunDecList();
    FunDec* aux;
    aux = parseFunDec();
    while (aux != NULL) {
        fdl->add(aux);
        aux = parseFunDec();
    }
    return fdl;
}

Program* Parser::parseProgram() {
    VarDecList* v = parseVarDecList();
    FunDecList* b = parseFunDecList();
    return new Program(v, b);
}

list<Stm*> Parser::parseStmList() {
    list<Stm*> slist;
    slist.push_back(parseStatement());
    while(match(Token::PC)) {
        slist.push_back(parseStatement());
    }
    return slist;
}

Stm* Parser::parseStatement() {
    Stm* s = NULL;
    Exp* e = NULL;
    Body* tb = NULL; //true case
    Body* fb = NULL; //false case

    if (current == NULL) {
        cout << "Error: Token actual es NULL" << endl;
        exit(1);
    }

    if (match(Token::ID)) {
        string lex = previous->text;

        if (match(Token::ASSIGN)) {
            e = parseCExp();
            s = new AssignStatement(lex, e);
        } else if (match(Token::PI)) {
            // Llamada a función como sentencia
            list<Exp*> args;
            if (!check(Token::PD)) {
                args.push_back(parseCExp());
                while (match(Token::COMA)) {
                    args.push_back(parseCExp());
                }
            }
            if (!match(Token::PD)) {
                cout << "Error: se esperaba ')' después de la lista de argumentos." << endl;
                exit(1);
            }
            s = new FCallStatement(lex, args);
        } else {
            cout << "Error: se esperaba '=' o '(' después del identificador." << endl;
            exit(1);
        }
    } else if (match(Token::PRINTF)) {
        if (!match(Token::PI)) {
            cout << "Error: se esperaba un '(' después de 'printf'." << endl;
            exit(1);
        }
        if(!match(Token::STRING)){
            cout << "Error: se esperaba un string después de 'print'." << endl;
            exit(1);
        }
        string texto = previous->text; //Guardo el string para el print
        if (!match(Token::COMA))
        {
            cout << "Error: se esperaba ',' después del string." << endl;
            exit(1);
        }
        e = parseCExp();
        if (!match(Token::PD)) {
            cout << "Error: se esperaba un ')' después de la expresión." << endl;
            exit(1);
        }
        s = new PrintStatement(e);
    }
    else if (match(Token::IF)) {
        if (!match(Token::PI)){
            cout << "Error: se esperaba '(' después de la expresión." << endl;
        }
        e = parseCExp();
        if (!match(Token::PD)) {
            cout << "Error: se esperaba ')' después de la expresión." << endl;
            exit(1);
        }

        if (!match(Token::LBRACE)) {
            cout << "Error: se esperaba '{' después de la expresión." << endl;
            exit(1);
        }

        tb = parseBody();
        if (!match(Token::RBRACE)) {
            cout << "Error: se esperaba '}' al final de la declaración." << endl;
            exit(1);
        }

        if (!match(Token::ELSE)) {
            cout << "Error: se esperaba 'else' después del bloque de 'if'." << endl;
            exit(1);
        }
        if (!match(Token::LBRACE)) {
            cout << "Error: se esperaba '{' después de la expresión." << endl;
            exit(1);
        }
        fb = parseBody();
        if (!match(Token::RBRACE)) {
            cout << "Error: se esperaba '}' al final de la declaración." << endl;
            exit(1);
        }
        s = new IfStatement(e, tb, fb);

    }
    else if (match(Token::WHILE)) {
        e = parseCExp();
        if (!match(Token::DO)) {
            cout << "Error: se esperaba 'do' después de la expresión." << endl;
            exit(1);
        }
        tb = parseBody();
        if (!match(Token::ENDWHILE)) {
            cout << "Error: se esperaba 'endwhile' al final de la declaración." << endl;
            exit(1);
        }
        s = new WhileStatement(e, tb);

    }
    else if(match(Token::FOR)){
        if(!match(Token::PI)){
            cout << "Error: se esperaba '(' después de 'for'." << endl;
            exit(1);
        }
        Exp* start = parseCExp();
        if (!match(Token::COMA)) {
            cout << "Error: se esperaba ',' después de la expresión." << endl;
            exit(1);
        }
        Exp* end = parseCExp();
        if (!match(Token::COMA)) {
            cout << "Error: se esperaba ',' después de la expresión." << endl;
            exit(1);
        }
        Exp* step = parseCExp();
        if (!match(Token::PD)) {
            cout << "Error: se esperaba ')' después de la expresión." << endl;
            exit(1);
        }
        tb = parseBody();
        if (!match(Token::ENDFOR)) {
            cout << "Error: se esperaba 'endfor' al final de la declaración." << endl;
            exit(1);
        }
        s = new ForStatement(start, end, step, tb);
    }
    else if(match(Token::RETURN)){
        if (!match(Token::PI)) {
            cout << "Error: se esperaba '(' después de 'return'." << endl;
            exit(1);
        }
        if (!check(Token::PD)){
            e = parseCExp();
        }
        if (!match(Token::PD)) {
            cout << "Error: se esperaba ')' después de la expresión." << endl;
            exit(1);
        }
        s = new ReturnStatement(e); //Si es null, no hay problema
    }
    else {
        cout << "Error: Se esperaba un identificador o 'print', pero se encontró: " << *current << endl;
        exit(1);
    }
    return s;
}

Exp* Parser::parseCExp(){
    Exp* left = parseExpression();
    if (match(Token::LT) || match(Token::LE) || match(Token::EQ)){
        BinaryOp op;
        if (previous->type == Token::LT){
            op = LT_OP;
        }
        else if (previous->type == Token::LE){
            op = LE_OP;
        }
        else if (previous->type == Token::EQ){
            op = EQ_OP;
        }
        Exp* right = parseExpression();
        left = new BinaryExp(left, right, op);
    }
    return left;
}

Exp* Parser::parseExpression() {
    Exp* left = parseTerm();
    if(match(Token::INCREMENT) || match(Token::DECREMENT)){
        UnaryOp op;
        if (previous->type == Token::INCREMENT){
            op = INCREMENT_OP;
        }
        else if (previous->type == Token::DECREMENT){
            op = DECREMENT_OP;
        }
        //return new UnaryExp(left, op); //No se usa por el momento
    }
    while (match(Token::PLUS) || match(Token::MINUS)) {
        BinaryOp op;
        if (previous->type == Token::PLUS){
            op = PLUS_OP;
        }
        else if (previous->type == Token::MINUS){
            op = MINUS_OP;
        }
        Exp* right = parseTerm();
        left = new BinaryExp(left, right, op);
    }
    return left;
}

Exp* Parser::parseTerm() {
    Exp* left = parseFactor();
    while (match(Token::MUL) || match(Token::DIV)) {
        BinaryOp op;
        if (previous->type == Token::MUL){
            op = MUL_OP;
        }
        else if (previous->type == Token::DIV){
            op = DIV_OP;
        }
        Exp* right = parseFactor();
        left = new BinaryExp(left, right, op);
    }
    return left;
}

Exp* Parser::parseFactor() {
    Exp* e;
    Exp* e1;
    Exp* e2;

    if (match(Token::TRUE)){
        return new BoolExp(1);
    }else if (match(Token::FALSE)){
        return new BoolExp(0);
    }
    else if (match(Token::NUM)) {
        return new NumberExp(stoi(previous->text));
    }
    else if (match(Token::ID)) {
        string texto = previous->text;

        //Parse FCallExp
        if (match(Token::PI)){
            list<Exp*> args;
            if (!check(Token::PD)){
                args.push_back(parseCExp());
                while (match(Token::COMA)){
                    args.push_back(parseCExp());
                }
            }
            if (!match(Token::PD)){
                cout << "Falta paréntesis derecho" << endl;
                exit(0);
            }
            return new FCallExp(texto, args);
        }
        else{
            return new IdentifierExp(previous->text);
        }

    }
    else if (match(Token::IFEXP)) {
        match(Token::PI);
        e=parseCExp();
        match(Token::COMA);
        e1=parseCExp();
        match(Token::COMA);
        e2=parseCExp();
        match(Token::PD);
        return new IFExp(e,e1,e2);
    }
    else if (match(Token::PI)){
        e = parseCExp();
        if (!match(Token::PD)){
            cout << "Falta paréntesis derecho" << endl;
            exit(0);
        }
        return e;
    }
    cout << "Error: se esperaba un número o identificador." << endl;
    exit(0);
}

