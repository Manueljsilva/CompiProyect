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
    if (match(Token::TYPE)) {
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

    while (!check(Token::RBRACE) && !isAtEnd()) {
        Stm* stmt = parseStatement();
        if (stmt != NULL) {
            sl->add(stmt);

            // Verificar si el statement actual es un if-else
            bool isIfStatement = dynamic_cast<IfStatement*>(stmt) != nullptr||
                                dynamic_cast<ForStatement*>(stmt) != nullptr ||
                                dynamic_cast<WhileStatement*>(stmt) != nullptr;

            // No requerir punto y coma después de bloques if-else o antes de else
            if (!check(Token::RBRACE) && !isIfStatement) {
                if (!match(Token::PC)) {
                    cout << "Error: se esperaba ';' después del statement" << " pero se tiene un " << current->type <<endl;
                    exit(1);
                }
            }
        }
    }

    return sl;
}


Body* Parser::parseBody() {
    if (!match(Token::LBRACE)) {
        cout << "Error: se esperaba '{' al inicio del cuerpo." << endl;
        exit(1);
    }
    VarDecList* vdl = parseVarDecList();
    StatementList* sl = parseStatementList();
    if (!match(Token::RBRACE)) {
        cout << "Error: se esperaba '}' al inicio del cuerpo." << "pero sale:" << current->text << endl;
        exit(1);
    }
    return new Body(vdl, sl);
}


FunDec* Parser::parseFunDec() {
    FunDec* fd = NULL;
    if (match(Token::TYPE)) {
        string returnType = previous->text;
        if (!match(Token::MAIN) && !match(Token::ID)) {
            cout << "Error: se esperaba 'main' o un nombre de función después del tipo." << endl;
            exit(1);
        }
        string funcName = previous->text;
        bool isMain = (funcName == "main");

        if (!match(Token::PI)) {
            cout << "Error: se esperaba '(' después del nombre de la función." << endl;
            exit(1);
        }

        list<string> paramTypes;
        list<string> paramNames;
        if (!check(Token::PD)) {
            if (!match(Token::TYPE)) {
                cout << "Error: se esperaba un tipo de parámetro." << endl;
                exit(1);
            }
            paramTypes.push_back(previous->text);

            if (!match(Token::ID)) {
                cout << "Error: se esperaba un nombre de parámetro." << endl;
                exit(1);
            }
            paramNames.push_back(previous->text);

            while (match(Token::COMA)) {
                if (!match(Token::TYPE)) {
                    cout << "Error: se esperaba un tipo de parámetro después de ','." << endl;
                    exit(1);
                }
                paramTypes.push_back(previous->text);

                if (!match(Token::ID)) {
                    cout << "Error: se esperaba un nombre de parámetro." << endl;
                    exit(1);
                }
                paramNames.push_back(previous->text);
            }
        }

        if (!match(Token::PD)) {
            cout << "Error: se esperaba ')' después de los parámetros." << endl;
            exit(1);
        }
        Body* body = parseBody();
        fd = new FunDec(funcName, paramTypes, paramNames, returnType, body);
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

    FunDecList* fdl = new FunDecList();
    VarDecList* vdl = new VarDecList();
    bool mainFound = false;

    // Parsear declaraciones de funciones hasta encontrar EOF
    while (!isAtEnd()) {
        if (match(Token::TYPE)) {
            string returnType = previous->text;

            // Verificar si es main o una función normal
            if (match(Token::MAIN)) {
                if (mainFound) {
                    cout << "Error: múltiples definiciones de main" << endl;
                    exit(1);
                }
                mainFound = true;

                // Parsear main()
                if (!match(Token::PI) || !match(Token::PD)) {
                    cout << "Error: se esperaba '()' después de main" << endl;
                    exit(1);
                }

                Body* mainBody = parseBody();


                fdl->add(new FunDec("main", list<string>(), list<string>(), returnType, mainBody));

            } else if (match(Token::ID)) {
                // Parsear función normal
                string funcName = previous->text;

                if (!match(Token::PI)) {
                    cout << "Error: se esperaba '(' después del nombre de la función" << endl;
                    exit(1);
                }

                // Parsear parámetros
                list<string> paramTypes;
                list<string> paramNames;

                if (!check(Token::PD)) {
                    if (!match(Token::TYPE)) {
                        cout << "Error: se esperaba tipo de parámetro" << endl;
                        exit(1);
                    }
                    paramTypes.push_back(previous->text);

                    if (!match(Token::ID)) {
                        cout << "Error: se esperaba nombre de parámetro" << endl;
                        exit(1);
                    }
                    paramNames.push_back(previous->text);

                    while (match(Token::COMA)) {
                        if (!match(Token::TYPE)) {
                            cout << "Error: se esperaba tipo de parámetro" << endl;
                            exit(1);
                        }
                        paramTypes.push_back(previous->text);

                        if (!match(Token::ID)) {
                            cout << "Error: se esperaba nombre de parámetro" << endl;
                            exit(1);
                        }
                        paramNames.push_back(previous->text);
                    }
                }

                if (!match(Token::PD)) {
                    cout << "Error: se esperaba ')'" << endl;
                    exit(1);
                }
                Body* funcBody = parseBody();
                fdl->add(new FunDec(funcName, paramTypes, paramNames, returnType, funcBody));
            }
        } else {
            cout << "Error: se esperaba tipo de retorno" << endl;
            exit(1);
        }
    }

    if (!mainFound) {
        cout << "Error: no se encontró la función main" << endl;
        exit(1);
    }

    return new Program(vdl, fdl);
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
    Body* tb = NULL; // true case
    Body* fb = NULL; // false case

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
            list<Exp*> args;
            if (!check(Token::PD)) {
                args.push_back(parseCExp());
                while (match(Token::COMA)) {
                    args.push_back(parseCExp());
                }
            }
            if (!match(Token::PD)) {
                cout << "Error: se esperaba ')' despues de la lista de argumentos." << endl;
                exit(1);
            }
            s = new FCallStatement(lex, args);
        } else if (match(Token::INCREMENT) || match(Token::DECREMENT)){
            UnaryOp op = (previous->type == Token::INCREMENT) ? INCREMENT_OP : DECREMENT_OP;
            s = new AssignStatement(lex, new UnaryExp(new IdentifierExp(lex), op));
        } else {
            cout << "Error: se esperaba '=' o '(' despues del identificador " << "pero se vio" << current->text << endl;
            exit(1);
        }
    } else if (match(Token::PRINTF)) {
        if (!match(Token::PI)) {
            cout << "Error: se esperaba '(' después de printf" << endl;
            exit(1);
        }

        if (!match(Token::PRINTF_FORMAT)) {
            cout << "Error: se esperaba formato de printf" << endl;
            exit(1);
        }
        string format = previous->text;
        Exp* e = nullptr;

        if (match(Token::COMA)) {
            e = parseCExp();
        }

        if (!match(Token::PD)) {
            cout << "Error: se esperaba ')' después de printf" << endl;
            exit(1);
        }

        s = new PrintStatement(e, format);
    } else if (match(Token::IF)) {
        if (!match(Token::PI)){
            cout << "Error: se esperaba '(' después de la expresión." << endl;
        }
        Exp* condition = parseCExp();
        if (!match(Token::PD)) {
            cout << "Error: se esperaba ')' despues de la expresion." << "pero sale: " << current->text << endl;
            exit(1);
        }

        Body* thenBody = parseBody();

        Body* elseBody = nullptr;
        if (match(Token::ELSE)) {


            elseBody = parseBody();


        }

        return new IfStatement(condition, thenBody, elseBody);

    } else if (match(Token::WHILE)) {
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

    }else if(match(Token::FOR)){
        if(!match(Token::PI)){
            cout << "Error: se esperaba '(' después de 'for'." << endl;
            exit(1);
        }
        VarDec* init = parseVarDec();
        Exp* condition = parseCExp();

        if (!match(Token::PC)) {
            cout << "Error: se esperaba ';' después de la condicion." << "pero se vio " <<current->type <<endl;
            exit(1);
        }
        Stm* increment = parseStatement();
        if (!match(Token::PD)) {
            cout << "Error: se esperaba ')' después de la expresión." << endl;
            exit(1);
        }
        tb = parseBody();

        s = new ForStatement(init, condition, increment, tb);
    } else if(match(Token::RETURN)){

        if (!check(Token::PD)){
            e = parseCExp();
        }

        s = new ReturnStatement(e); //Si es null, no hay problema
    }else {
        cout << "Error: Se esperaba un identificador o 'print', pero se encontró: " << *current << endl;
        exit(1);
    }
    return s;
}

Exp* Parser::parseCExp(){
    Exp* left = parseExpression();

    if (match(Token::LT) || match(Token::LE) || match(Token::EQ) || match(Token::GT)){
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
        else if (previous->type == Token::GT){
            op = GT_OP;
        }
        Exp* right = parseExpression();
        left = new BinaryExp(left, right, op);
    }
    return left;
}

Exp* Parser::parseExpression() {
    Exp* left = parseTerm();
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

